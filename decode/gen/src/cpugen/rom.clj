(ns cpugen.rom
  (:use cpugen.vmagic)
  (:require [clojure.string :as s]
            [cpugen [logic :as logic]])
  (:import
   [de.upb.hni.vmagic.object
    Signal]
   [de.upb.hni.vmagic.literal
    AbstractLiteral
    BasedLiteral
    BinaryLiteral
    CharacterLiteral
    DecimalLiteral
    EnumerationLiteral
    HexLiteral
    Literals
    OctalLiteral
    PhysicalLiteral
    StringLiteral]
   [de.upb.hni.vmagic.object
    ArrayElement]))

(defn log2 [n]
  (- Long/SIZE (Long/numberOfLeadingZeros (dec n))))

(defn to-bin-num [n width]
  (let [s (Long/toBinaryString n)]
    (str (apply str (repeat (- width (count s)) \0)) s)))

(defn combinable-signals
  "Returns a list of sets for a target ROM width. All the possible
values for each of the keys in a particular set are encoded in the
same bits of the ROM line.

If the value of several control lines are correlated, then the set of
all values of those control lines can be encoded in fewer bits than it
would take to encode each control line's possible values individually.
There is a trade-off of course. It takes more logic to decode the
microcode when control lines share the same bits."

  [width]
  (case width
    64
    [#{:wrreg-w :wrreg-z
       :regnum-w :regnum-z}
     #{:alumanip :shiftfunc :arith-func :logic-func}
     #{:sr-sel :t-sel :arith-sr-func :logic-sr-func}
     #{:regnum-x :xbus-sel :aluinx-sel}
     #{:regnum-y :ybus-sel}
     #{:ma-issue :ma-wr :mem-size :mem-addr-sel :mem-wdata-sel}
     #{:ex-macsel1 :wb-macsel1
       :ex-mulcom1 :wb-mulcom1
       :ex-wrmacl :wb-wrmacl}
     #{:ex-macsel2 :wb-macsel2
       :ex-mulcom2 :wb-mulcom2
       :ex-wrmach :wb-wrmach}
     ;;#{:if-issue :ifadsel :dispatch :delay-jump :incpc}
     #{:if-issue :dispatch}
     ;;   #{:ex-wrmach :ex-wrmacl :wb-wrmach :wb-wrmacl}
     #{:wrpc-z :wrpr-pc :wrsr-w :wrsr-z}]
    72
    [#{:alumanip :shiftfunc :arith-func :logic-func}
     #{:sr-sel :t-sel :arith-sr-func :logic-sr-func}
     #{:regnum-y :ybus-sel :aluiny-sel}
     #{:ma-issue :ma-wr :mem-size #_:mem-addr-sel}
     #{:ex-macsel1 :wb-macsel1
       :ex-mulcom1 :wb-mulcom1
       :ex-wrmacl :wb-wrmacl}
     #{:ex-macsel2 :wb-macsel2
       :ex-mulcom2 :wb-mulcom2
       :ex-wrmach :wb-wrmach}
     #{:wrpc-z :wrpr-pc :wrsr-w :wrsr-z}]))

(defn- create-encoding [slots nillable-outputs rom-width]
  (let [all-combines (apply hash-set (apply concat (combinable-signals rom-width)))
        key-fns (concat
                 (->> slots
                      (mapcat keys)
                      distinct
                      sort
                      (filter (complement all-combines))
                      (map (fn [k] {:key-fn (fn [slot] [(slot k)])
                                   :keys [k]})))
                 (map
                  (fn [sig-keys]
                    (let [sig-keys (vec sig-keys)]
                      {:key-fn (fn [slot] (vec ((apply juxt sig-keys) slot)))
                       :keys sig-keys}))
                  (combinable-signals rom-width)))
        encodings (map
                   (fn [enc]
                     (let [key-fn (:key-fn enc)
                           possible-vals (->> slots
                                              (map key-fn)
                                              distinct
                                              ;; filter out single nil
                                              ;; values if keys is a
                                              ;; single nillable key
                                              (filter
                                               (if (and (= 1 (count (:keys enc)))
                                                        (nillable-outputs (first (:keys enc))))
                                                 #(not= [nil] %)
                                                 (constantly true))))
                           encode-width (log2 (count possible-vals))]
                       (assoc enc
                         :encode-width encode-width
                         :encoding (into {}
                                         (map-indexed
                                          (fn [i v]
                                            [v (to-bin-num i encode-width)])
                                          ;; make sure nil is first if
                                          ;; it is present
                                          (let [multi-nil?
                                                (fn [x] (= x (repeat (count (:keys enc)) nil)))]
                                            
                                            (concat
                                             (filter multi-nil? possible-vals)
                                             (filter (complement multi-nil?) possible-vals))))))))
                   key-fns)
        full-width (apply + (map :encode-width encodings))
        ;; calculate bit offsets
        [encodings _] (reduce
                       (fn [[encodings i] enc]
                         (let [width (:encode-width enc)]
                           [(conj encodings
                                  (assoc enc
                                    :left i
                                    :right (inc (- i width))))
                            (- i width)]))
                       [[] (dec full-width)]
                       encodings)]
    [full-width encodings]))

(defn line-encoder [slots nillable-outputs rom-width]
  (let [[full-width encodings] (create-encoding slots nillable-outputs rom-width)]
    (println "NUM ENCODINGS" (count encodings) "full width" full-width)
    #_(doseq [enc (filter #(some #{:regnum-x :regnum-y} (:keys %)) encodings)]
      (println "Encoding of" (:keys enc) (:encoding enc)))
    #_(doseq [enc encodings]
      (println "Encoding of" (:keys enc) (:encoding enc)))
    [(std-logic-vector full-width)
     ;; encoder
     (fn [slot]
       (let [s (s/join (map
                        (fn [{:keys [keys key-fn encoding encode-width]}]
                          (or (encoding (key-fn slot))
                              (s/join (repeat encode-width \0))))
                        encodings))]
         ;; encode as a hex string if possible
         (if (zero? (mod full-width 4))
           (HexLiteral.
            (s/join
             (map
              #(Long/toHexString (Long/parseLong (s/join %) 2))
              (partition 4 s))))
           (StringLiteral. s))))
     ;; decoder
     (fn [line-sig controls transforms]
       (mapcat
        (fn [{:keys [left right encoding keys]}]
          (map
           (fn [i k]
             (let [sig (controls k)
                   width (inc (- right left))]
               (if (and (= 1 width) (= std-logic (type-of sig)))
                 ;; single bit of line
                 (cond-assign sig (ArrayElement. line-sig left))

                 (let [zero (zero-val (controls k))
                       val-map (->> encoding
                                    (filter (fn [[ks _]]
                                              (nth ks i)))
                                    (group-by #(nth (first %) i))
                                    (map (fn [[k vals]]
                                           [k (map second vals)]))
                                    (into {}))]
                   (apply select-assign
                          sig
                          (slice-downto line-sig left right)
                          (concat
                           (->> val-map
                                (sort-by count)
                                (mapcat
                                 (fn [[val bits]]
                                   (let [val ((or (get transforms k) identity) val)
                                         val (if (number? val) (num-val sig val) val)]
                                     (when (not (v-equals val zero))
                                       [val
                                        (mapv #(StringLiteral. %) bits)])))))
                           [zero]))))))
           (range)
           keys))
        (sort-by :left encodings)))]))

(defn- order-cost [ops addr-width]
  (let [ops (map #(assoc %1 :index %2) ops
                 (reductions + 0 (map (comp count :slots) ops)))
        conditions
        (map
         (fn [i]
           (->> ops
                (group-by #(zero? (bit-and (bit-shift-left 1 i) (:index %))))
                vals
                (map (fn [ops]
                       (->> ops
                            (map :logic-map)
                            (filter identity)
                            logic/reduce-implicants
                            (map #(apply hash-set %)))))
                ;; order by some cost function
                (sort-by #(apply + (map count %)))
                first))
         (range addr-width))]
    (apply + (mapcat #(map count %) conditions))))

(defn optimize-microcode-order
  "Choose an order for instructions in microcode to try to minimize
the necessary predecoder logic."
  [ops addr-width]
  (let [mem-size (long (Math/pow 2 addr-width))
        ops (map-indexed (fn [i op] (assoc op :order i)) ops)
        microcode-lengths (map (comp count :slots) ops)
        total-len (apply + microcode-lengths)
        ops (map #(assoc % :logic-map (logic/op-to-logic-map %)) ops)]
    (if (< mem-size total-len)
      (throw (IllegalArgumentException. (str "memory size (" mem-size ") too small to hold "
                                             total-len " microcode instructions"))))

    (let [ops (vec (concat ops (repeat (- mem-size total-len) {:slots [nil]})))
          ops (shuffle ops)]
      (println "default order" (order-cost ops addr-width))

      (let [num-runs 500
            best
            (loop [best ops
                   best-cost (order-cost ops addr-width)
                   current ops
                   current-cost (order-cost ops addr-width)
                   n (long 0)]
              (if (zero? (mod  n 10)) (println "n:" n current-cost))
              (if (< n num-runs)
                (let [
                      ;; swap two neighbouring instructions..
                      ;; probably should make bigger swaps when
                      ;; temperature is high
                      swap-index (rand-int (dec (count current)))
                      ops (assoc current
                            swap-index (current (inc swap-index))
                            (inc swap-index) (current swap-index))
                      ;;ops (shuffle ops)
                      cost (order-cost ops addr-width)
                      best (if (< cost best-cost)
                             best ops)
                      best-cost (if (< cost best-cost)
                                  best-cost cost)]
                  ;; use simple threshhold to accept worse solutions
                  (if (< (- cost (/ (* 100 n) num-runs))
                         current-cost)
                    (recur best best-cost ops cost (inc n))
                    (recur best best-cost current current-cost (inc n))))
                best))]
        (println "best cost: " (order-cost (shuffle best) addr-width))))))

(defn- aligned-gaps [^ints obj-array gap-size]
  (filter
   (fn [i]
     (->> (range i (+ i gap-size))
          (map (fn [i]
                 (aget obj-array i)))
          (every? zero?)))
   (range 0 (alength obj-array) gap-size)))

(defn- round-up-pow-2 [x]
  (let [high (Long/highestOneBit x)]
    (if (= x high)
      high
      (bit-shift-left high 1))))

(defn reorder-microcode [ops method]
  (case method
    :nop
    ;; use given order
    (let [ops (map (fn [op index] (assoc op :index index))
                   ops
                   (reductions + 0 (map (comp count :slots) ops)))]

      ops)
    :align16

    ;; order so that the instr_seq can be ORed together instead of
    ;; greedily place largest instructions first
    (let [gap-array (int-array 256)
            ops-by-size (->> ops
                             (partition-by (comp count :slots))
                             (group-by (comp count :slots first)))
            ops
            (reduce
             (fn [ops op-len]
               (let [ops-of-len (flatten (get ops-by-size op-len))
                     gaps (aligned-gaps gap-array (round-up-pow-2 op-len))
                     gaps (take (count ops-of-len) gaps)]
                 (if (< (count gaps) (count ops-of-len))
                   (throw (IllegalStateException.
                           (str "Unable to find enough gaps of size " op-len))))
                 (doseq [gap gaps
                         i (range gap (+ gap op-len))]
                   (aset gap-array i 1))
                 (into ops
                       (map
                        (fn [gap-start op]
                          (assoc op :index gap-start))
                        gaps
                        ops-of-len))))
             [] (reverse (sort (keys ops-by-size))))]
        ;; Ensure address 255 isn't used. It's used to signal an illegal instruction
        (when (some #(= 255 (:index %)) ops)
          (throw (IllegalStateException. "Packed ROM with instruction at resrved address 0xFFFF")))
        ops)))
