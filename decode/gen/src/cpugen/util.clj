(ns cpugen.util
  (:require [cpugen
             [logic :as logic]]
            [clojure.string :as s])
  (:import
   [de.upb.hni.vmagic
    Choices]
   [de.upb.hni.vmagic.libraryunit
    Architecture]
   [de.upb.hni.vmagic.builtin
    Libraries
    NumericStd
    SignalAttributes
    Standard
    StdLogic1164
    StdLogicArith
    StdLogicSigned
    StdLogicUnsigned]
   [de.upb.hni.vmagic.object
    Variable]
   [de.upb.hni.vmagic.declaration
    FunctionDeclaration
    FunctionBody]
   [de.upb.hni.vmagic.literal
    HexLiteral]
   [de.upb.hni.vmagic.concurrent
    ProcessStatement])
  (:use cpugen.vmagic))

(defn- write-pc-op? [x]
  (boolean
   (or (#{:pc :pc-t :pc-nt} x)
       (= "PC" (:name (meta x))))))

(defn illegal-slot-imps
  "Returns sequence of logic map implicants that will detect illegal slot instructions"
  [ops]
  (let [{write-pc-ops true legal-delay-ops nil}
        (->> ops
             (map #(assoc % :logic-map (logic/str-to-logic-map :i (:op %))))
             (filter #(= 0 (:plane %)))
             (group-by (fn [{slots :slots}]
                         (some
                          (fn [{z :z w :w}]
                            (or (write-pc-op? z)
                                (write-pc-op? w)))
                          slots))))
        constraint-sets
        (map
         #(apply hash-set (logic/str-to-logic-map :i (:op %)))
         legal-delay-ops)
        imps
        (->> write-pc-ops
             (map :logic-map)
             logic/reduce-implicants
             (map #(into {} (logic/relax-logic-set (apply hash-set %) constraint-sets)))
             logic/reduce-implicants)]
    [write-pc-ops imps]
    #_(doseq [imp imps]
      (println (logic/logic-map-to-stdmatch imp :i 16)
               #_(vmagic/vstr (logic/logic-map-to-bool
                             imp {:i (vmagic/signal "if_dr" (vmagic/std-logic-vector 16))}))
               (s/join ", " (flatten (->> write-pc-ops
                                          (filter #(logic/intersects? imp (:logic-map %)))
                                          (map :name))))))))

(defn gen-illegal-slot-fn
  [ops fn-decl]
  (let [[write-pc-ops imps] (illegal-slot-imps ops)
        code-input (first (.getParameters fn-decl))]
    (-> (FunctionBody. fn-decl)
        (add-all statements
                 (set-comments
                  (if-stmt
                   (apply v-or
                          (map
                           #(logic/logic-map-to-bool
                             % {:i code-input})
                           imps))
                   (return-stmt StdLogic1164/STD_LOGIC_1)
                   (return-stmt StdLogic1164/STD_LOGIC_0))
                  "Check for instructions that assign to PC:"
                  (s/join ", " (map :name write-pc-ops)))))))

(defn gen-illegal-instr-fn
  [ops fn-decl]
  (let [code-input (first (.getParameters fn-decl))]
    (-> (FunctionBody. fn-decl)
        (add-all statements
                 (set-comments
                  (if-stmt
                   (v= (slice-downto code-input 15 8)
                       (HexLiteral. "ff"))
                   (return-stmt StdLogic1164/STD_LOGIC_1)
                   (return-stmt StdLogic1164/STD_LOGIC_0))
                  "TODO: Improve detection of illegal instructions")))))

(defn- to-predecode-conditions
  "Given seq of ops, returns bit-width number of [on off] pairs where
  on and off are lists of implicants for setting the corresponding bit
  in an address to either 1 or 0"
  [bit-width ops]
  (->> (range bit-width)
       (map #(bit-shift-left 1 %))
       (map
        (fn [i]
          (let [{on true off false}
                (group-by
                 #(not (zero? (bit-and i (:index %))))
                 ops)
                ;;_ (println "num on" (count on) "off" (count off))
                on (when (seq on)
                     (logic/reduce-implicants (map :logic-map on)))
                off (when (seq off)
                      (logic/reduce-implicants (map :logic-map off)))]
            [on off])))))

(def ops-atom (atom nil))

(defn gen-predecode-fn
  "Returns vhdl function body implementing the ROM predecode"
  [ops fn-decl]
  (let [code-input (first (.getParameters fn-decl))
        addr-output (Variable. "addr" (std-logic-vector 8))
        ops (->> ops
                 ;; only normal SH-2 instructions
                 (filter #(= 0 (:plane %)))
                 ;; remove plane from logic map
                 (map (fn [op] (assoc op :logic-map (dissoc (:logic-map op) [:p 0])))))
        conditions (map
                    (fn [[on off]]
                      (if (<= (count on) (count off))
                        {:invert false :imps on :ops ops}
                        {:invert true :imps off :ops ops}))
                    (to-predecode-conditions 8 ops))
        _ (reset! ops-atom ops)


        line-conditions (->> ops
                             ;; Remove first 4 bits, the line, from the logic
                             ;; maps. We'll check that with a case statement
                             (map
                              (fn [op]
                                (assoc op :logic-map
                                       (apply dissoc (:logic-map op)
                                              (for [i (range 12 16)] [:i i])))))
                             (group-by
                              #(Integer/parseInt
                                (first
                                 (s/split
                                  (:op %)
                                  #" +"))
                                2))
                             (map (fn [[line ops]]
                                    [line [ops
                                           (map
                                            (fn [[on off]]
                                              (let [n1 (count on)
                                                    n0 (count off)]
                                                (cond
                                                 (nil? on) {:invert true :imps nil}
                                                 (nil? off) {:invert false :imps nil}
                                                 :else
                                                 (if (or (zero? n1) (<= n0 n1))
                                                   {:invert true :imps off}
                                                   {:invert false :imps on}))))
                                            (to-predecode-conditions 8 ops))]]))
                             (into {}))
        line-conditions (merge
                         (into {} (map (fn [i] [i nil]) (range 16)))
                         line-conditions)
        ;; cache calculated implicats in separate signals. This didn't
        ;; improve the area though, so disable for now.
        imp-cache {} #_(->> conditions
                       (mapcat :imps)
                       frequencies
                       (filter #(> (second %) 1))
                       (map first)
                       (map-indexed
                        (fn [i imp]
                          [imp {:index i
                                :signal (signal (str "imp_bit_" i) std-logic)
                                :val (logic/logic-map-to-bit imp {:i code-input})}]))
                       (into {}))
        assign-addr (fn [conditions imp-cache]
                      (if (seq conditions)
                        (map-indexed
                         (fn [i {:keys [invert imps ops]}]
                           (let [conds (->> imps
                                            (map
                                             (fn [imp]
                                               (if-let [{sig :signal} (imp-cache imp)]
                                                 sig
                                                 (logic/logic-map-to-bit
                                                  imp {:i code-input})))))
                                 conds
                                 (case (count conds)
                                   0 (if invert
                                       StdLogic1164/STD_LOGIC_0
                                       StdLogic1164/STD_LOGIC_1)
                                   1 ((if invert v-not identity) (paren (first conds)))
                                   ((if invert v-not identity) (apply v-or conds)))]
                             (varassign (.getArrayElement addr-output i) conds)))
                         conditions)))]
    (-> (FunctionBody. fn-decl)
        (add-declarations
         addr-output
         (map :signal (sort-by :index (vals imp-cache))))
        (add-all statements
                 (map #(cond-assign (:signal %) (:val %))
                      (sort-by :index (vals imp-cache)))
                 ;; choose the style of addr assignments
                 (case :by-line
                   ;; directly assign each bit of the address once
                   :direct
                   (assign-addr conditions imp-cache)
                   ;; first 
                   :by-line
                   (apply case-statement (slice-downto code-input 15 12)
                          (concat
                           (mapcat (fn [[line [ops conds]]]
                                     (let [num-ops (count ops)
                                           assigns
                                           (case num-ops
                                             0 nil
                                             1 [(varassign addr-output (:index (first ops)))]
                                             (assign-addr conds imp-cache))]
                                       (when assigns
                                         [(HexLiteral. line)
                                          (apply set-comments assigns
                                                 (map (fn [op]
                                                        (str (:op op)
                                                             " => "
                                                             (.replace
                                                              (String/format
                                                               "%8s"
                                                               (into-array Object
                                                                           [(Integer/toString (:index op) 2)]))
                                                              \ \0)
                                                             "  "
                                                             (:name op)))
                                                      (sort-by :op ops)))])))
                                   (sort-by first line-conditions))
                           [Choices/OTHERS
                            [(varassign addr-output 255)]])))
                 (return-stmt addr-output)))
    ;; This was an attempt to make '1' the default for each bit so that
    ;; addr is assigned 0xFF for illegal instructions. Problem is it's
    ;; bigger and slower. Reording the ROM should help with that.
    #_(-> (Architecture. "arch" entity)
          (add-all statements
                   (map
                    (fn [i]
                      (cond-assign (.getArrayElement addr-output i)
                                   (v-not
                                    (->> ops
                                         (filter #(zero? (bit-and (bit-shift-left 1 i) (:index %))))
                                         (map :logic-map)
                                         logic/reduce-implicants
                                         (map #(logic/logic-map-to-bit % {:i code-input}))
                                         (apply v-or)))))
                    (range 8))))))
