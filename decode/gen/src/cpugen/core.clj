(ns cpugen.core
  (:require [cpugen
             [parser :as parser]
             [logic :as logic]
             [genvhdl :as genvhdl]
             [vhdlmicrocode :as vhdlmc]
             [rom :as rom]
             [vmagic :as vmagic]
             [vcd :as vcd]]
            [clojure.string :as s]
            clojure.stacktrace)
  (:use [clojure.core.match :only (match)])
  (:import
   de.upb.hni.vmagic.output.VhdlOutput))

(def file-name "SH-2 Instruction Set.ods")

(defn show-gen
  ([]
     (when-let [ops (parser/check-load file-name)]
       (show-gen ops)))
  ([ops]
     (let [{:keys [pkg decode]} (genvhdl/gen-decoder ops)]
       (println (vmagic/vstr decode)))))

(defn gen-decoder [& options]
  (try
    (when-let [ops (parser/check-load file-name)]
      (let [{:keys [pkg decode decode-core
                    decode-table
                    decode-simple decode-reverse decode-rom
                    decode-body
                    test-decode-pkg print-op]}
            (apply genvhdl/gen-decoder ops options)]
        (VhdlOutput/toFile decode "../decode.vhd")
        (VhdlOutput/toFile decode-table "../decode_table.vhd")
        (VhdlOutput/toFile decode-simple "../decode_table_simple.vhd")
        (VhdlOutput/toFile decode-reverse "../decode_table_reverse.vhd")
        (VhdlOutput/toFile decode-rom "../decode_table_rom.vhd")
        (VhdlOutput/toFile decode-body "../decode_body.vhd")
        (VhdlOutput/toFile pkg "../decode_pkg.vhd")
        ;;(VhdlOutput/toFile test-decode-pkg "../tests/test_decode_pkg.vhd")
        (spit "../../sim/sh2instr.c" print-op)))
    (catch Exception e (clojure.stacktrace/print-stack-trace e))))

(defn load-mcode-assigns []
  (->> (parser/check-load file-name)
       (mapcat
        (fn [op]
          (let [slots (:slots op)
                #_(comment num-slots (count slots)
                         seq-bits (- Long/SIZE (Long/numberOfLeadingZeros (dec num-slots)))
                         op-cond (merge (str-to-logic-map (:op op) :i)
                                        (when-let [t (:case-t op)]
                                          (str-to-logic-map (str t) :t))))]
            (map-indexed (fn [i mc]
                           (vhdlmc/gen-assign-map op mc)
                           #_(let [cond
                                 (merge op-cond
                                        (when (> seq-bits 0)
                                          (merge (str-to-logic-map (apply str (repeat seq-bits \0)) :s)
                                                 (str-to-logic-map (Long/toBinaryString i) :s))
                                          ))]
                             {:seq-bits seq-bits
                              :logic-map cond
                              :controls (vhdlmc/gen-assign-map op mc)}))
                         slots))))))

(defn parse-bin [s]
  (Long/parseLong s 2))

(defn calc-mask [op]
  {:match
   (parse-bin
    (s/join
     (map
      (fn [x]
        (case x
          \m "0"
          \n "0"
          \d "0"
          \i "0"
          \0 "0"
          \1 "1"
          \- "0"
          nil))
      op)))
   :mask
   (parse-bin
    (s/join
     (map
      (fn [x]
        (case x
          \m "0"
          \n "0"
          \d "0"
          \i "0"
          \0 "1"
          \1 "1"
          \- "0"
          nil))
      op)))})

(defn generate-ops [op-pattern]
  (let [op-pattern (s/replace op-pattern #" +" "")
        op-match (:match (calc-mask op-pattern))
        wildcard-indicies
        (mapv vector
              (range)
              (reverse
               (filter identity
                       (map-indexed (fn [i c]
                                      (when (= c \-)
                                        (dec (- (count op-pattern) i))))
                                    op-pattern))))
        num-wildcards (count (filter #{\-} op-pattern))]
    #_(println "num-wildcards" num-wildcards (range (long (Math/pow 2 num-wildcards))))
    #_(println "wildcard-indicies" wildcard-indicies)
    (map
     (fn [x]
       #_(println "x =" x)
       (reduce
        (fn [v [i shift]]
          (bit-or v (bit-shift-left (bit-and (bit-shift-right x i) 1) shift)))
        op-match
        wildcard-indicies))
     (range (long (Math/pow 2 num-wildcards))))))

(defn ops-with-masks []
  (->> (parser/check-load file-name)
       (filter #(= 0 (:plane %)))
       (map #(select-keys % [:op :name :format]))
       distinct
       (map
        #(merge % (calc-mask (:op %))))))

(defn parse-op-log []
  (let [ops (ops-with-masks)
        oplog
        (with-open [rdr (clojure.java.io/reader "ops_only.log")]
          (frequencies
           (map
            (fn [line]
              (let [[_ addr opcode description]
                    (re-matches
                     #"IF 0x([\dA-Fa-f]+) 0x([\dA-Fa-f]+) (.*)$"
                     line)]
                (Long/parseLong opcode 16)))
            (line-seq rdr))))]
    (println "num ops: " (count oplog) (apply + (vals oplog)))
    (println "compare op logs against spreadsheet")
    (doseq [op (keys oplog)]
      ;; match the op against one from the spreadsheet
      (let [matches
            (filter (fn [o] (= (:match o) (bit-and (:mask o) op))) ops)]
        (case (count matches)
          0 (println "no match for" op)
          1 nil
          (println (count matches) "matches for" op ":" (s/join " " (map :name matches))))))
    (println "compare spreadsheet against op logs")
    (doseq [op ops]
      ;; match each op from spreadsheet against one of the logged ones
      (let [matches
            (filter (fn [o] (= (:match op) (bit-and (:mask op) o))) (keys oplog))]
        (if (zero? (count matches))
          (println "no match for" op))))))

(defn find-matching-ops
  "Returns all opcodes that match the supplied pattern. Patterns look
  like ----10101---1100"
  [op-pattern]
  (let [possible-ops (generate-ops op-pattern)]
    (filter
     (fn [{:keys [match mask]}]
       (some
        #(= match (bit-and mask %))
        possible-ops))
     (ops-with-masks))))

(def illegal-slot-instructions
  ["0000----00-00011" ;; BSRF, BRAF
   "0000000000101011" ;; RTE
   "0100----00-01011" ;; JSR, JMP
   "10001--1--------" ;; BT, BF, BT/S, BF/S
   "101-------------" ;; BRA, BSR
   "11000011--------" ;; TRAPA
   ])

(defn compile-slot-instr-check []
  (doseq [s
          (->> illegal-slot-instructions
               (map (partial logic/str-to-logic-map :i))
               (logic/reduce-implicants)
               (map #(logic/logic-map-to-stdmatch % :i 16)))]
    (println s)))

(defn optimize-predecode []
  (let [ops (parser/check-load file-name)]
    (try
      (rom/optimize-microcode-order ops 8)
    (catch Exception e (clojure.stacktrace/print-stack-trace e)))))


(defn- op-to-str [opcode ops]
  (let [matches
        (filter (fn [o] (= (:match o) (bit-and (:mask o) opcode))) ops)
        op (first matches)
        ra (str "R" (bit-and (bit-shift-right opcode 8) 0xf))
        rb (str "R" (bit-and (bit-shift-right opcode 4) 0xf))]
    #_{"n" :n
            "m" :m
            "nm" :nm
            "md" :md
            "nd4" :nd4
            "nmd" :nmd
            "d8" :d8
            "d12" :d12
            "nd8" :nd8
            "i8" :i8
            "ni" :ni}
    (str
     (case (or (:format op) :unknown)
       :n (-> (:name op)
              (s/replace-first #"Rn" ra))
       :m (-> (:name op)
              (s/replace-first #"Rm" ra))
       :nm (-> (:name op)
               (s/replace-first #"Rn" ra)
               (s/replace-first #"Rm" rb))
       :md (-> (:name op)
               (s/replace-first #"Rm" rb))
       :nd4 (-> (:name op)
                (s/replace-first #"Rn" rb))
       :nmd (-> (:name op)
                (s/replace-first #"Rn" ra)
                (s/replace-first #"Rm" rb))
       :nd8 (-> (:name op)
                (s/replace-first #"Rn" ra))
       :ni (-> (:name op)
               (s/replace-first #"Rn" ra))
       :unknown "UNKNOWN"
       :0 (:name op)
       (str (:name op) " fmt=" (name (:format op))))
     
     " ["
     (.substring
      ;; add 0x10000 and substring to zero pad to 4 chars
      (Long/toHexString (+ opcode 0x10000))
      1) "]")))

(defn analyze-vcd
  "Analyze chipscope output"
  []
  (let [{:keys [wires time-vals]}
        (vcd/read-vcd "capture3.vcd")
        ops (ops-with-masks)
        wire-idents
        (->> wires
          (map
           (fn [[ident {name :name}]]
             (when-let [kw ({"/mcu0/UCPU/if_ad" :if-ad
                             "/mcu0/UCPU/u_bus/if_dr" :if-dr
                             "/mcu0/UCPU/slot" :slot
                             "/mcu0/UCPU/u_bus/mfetch_ma_issue_MUX_110_o" :ma-issue
                             "/mcu0/UCPU/if_issue" :if-issue
                             "/mcu0/genaic.SYS/r_cpuerr" :cpuerr}
                            name)]
               [ident kw])))
          (filter identity)
          (into {}))

        ;; filter time-vals for only the wires we care about
        time-vals
        (->> time-vals
             (map (fn [{:keys [time vals]}]
                    (let [vals
                          (->> vals
                               (map (fn [[n v]]
                                      (when-let [kw (wire-idents n)]
                                        [kw v])))
                               (filter identity)
                               (into {}))]
                      (when (seq vals)
                        {:time time
                         :vals vals})))))]


    (with-open [out (clojure.java.io/writer "capture3.vcd.out")]
      (binding [*out* out]
        (doseq [{:keys [time vals]} time-vals]
          (println (str "#" time))
          (doseq [[kw v] vals]
            (println "" (name kw) "="
                     (case kw
                       :if-dr (op-to-str v ops)
                       :if-ad (str "0x" (Long/toHexString (* 2 v)))
                       v))))))

    #_(println time-vals)))
