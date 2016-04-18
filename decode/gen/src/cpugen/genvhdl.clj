(ns cpugen.genvhdl
  (:require [clojure.string :as s]
            [clojure.set :as set]
            [clojure.math.combinatorics :as combo]
            [cpugen
             [vhdlmicrocode :as vhdlmc]
             [rom :as rom]
             [interface :as inter]
             [logic :as logic]
             [util :as util]
             [genc :as genc]])
  (:use cpugen.vmagic
        [clojure.core.match :only (match)])
  (:import
   de.upb.hni.vmagic.output.VhdlOutput
   [de.upb.hni.vmagic
    VhdlElement
    VhdlFile
    AssociationElement
    DiscreteRange
    Range
    Range$Direction
    Choice
    Choices
    WaveformElement]
   [de.upb.hni.vmagic.statement
    CaseStatement
    ForStatement
    IfStatement
    SignalAssignment]
   [de.upb.hni.vmagic.expression
    Expression
    Aggregate
    Concatenate
    Not
    TypeConversion]
   [de.upb.hni.vmagic.libraryunit
    Architecture
    Configuration
    Entity
    LibraryClause
    LibraryUnit
    LibraryUnitVisitor
    PackageBody
    PackageDeclaration
    UseClause]
   [de.upb.hni.vmagic.declaration
    FunctionDeclaration
    FunctionBody
    SignalDeclaration
    ConstantDeclaration
    VariableDeclaration
    Subtype
    Component
    Attribute]
   [de.upb.hni.vmagic.builtin
    Libraries
    NumericStd
    SignalAttributes
    Standard
    StdLogic1164
    StdLogicArith
    StdLogicSigned
    StdLogicUnsigned]
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
   [de.upb.hni.vmagic.type
    ConstrainedArray
    UnconstrainedArray
    IntegerType
    IndexSubtypeIndication
    RecordType
    EnumerationType]
   [de.upb.hni.vmagic.object
    Constant
    Slice
    VhdlObject
    VhdlObject$Mode
    VhdlObjectProvider
    Signal
    RecordElement
    ArrayElement
    AttributeExpression
    Variable]
   [de.upb.hni.vmagic.concurrent
    ProcessStatement
    ComponentInstantiation]))

(defn log2-ceil [x]
  (long (Math/ceil (/ (Math/log x) (Math/log 2)))))

(defn gen-control-assignments [assigns outputs]
  (let [assigns
        (->> assigns
             ;; create SignalAssignment statements for each [sig-key
             ;; value] pair in assigns.
             (map
              (fn [a]
                (cond
                 (vector? a)
                 (let [[key val] a
                       sig (outputs key)]
                   (when (nil? sig)
                     (throw (NullPointerException. (str "signal " key " is null"))))
                   (when (keyword? val)
                     (throw (NullPointerException. (str "signal " key " assigned invalid value "
                                                        val))))
                   (when (not (or (number? val)
                                  (some #(instance? % val) [Concatenate Not StringLiteral
                                                            HexLiteral EnumerationLiteral Signal])))
                     (throw (IllegalStateException.
                             (str "signal " key " assigned invalid value " val))))
                   (assign sig val))
                 (string? a) a
                 :else a)))
             (filter identity))]
     ;; add comments to following assignment
    (doseq [[comments stmts] (partition 2 1 (partition-by string? assigns))]
      (when (string? (first comments))
        (apply set-comments stmts comments)))
    (filter (complement string?) assigns)))

(defn transform-values [assigns val-transforms]
  (doall
     (map
      (fn [a]
        (cond
         (vector? a)
         (let [[key val] a
               final-val ((or (get val-transforms key) identity) val)]
           (when (nil? final-val)
             (if (nil? val)
               (throw (NullPointerException. (str "control " key " assigned null value")))
               (throw (NullPointerException. (str "control " key " assigned value "
                                                  val " maps to null" (get val-transforms key))))))
           [key final-val])
         :else a))
      assigns)))

(defn gen-op-case [cond-var op instr-seq controls transforms]
  [(func-call-pos
    std-match
    cond-var
    (StringLiteral.
     (logic/logic-map-to-stdmatch
      (:logic-map op)
      :p 1 :i 16)))
   (let [cs (CaseStatement. instr-seq)
         instr-op (apply str
                         (map #(if-let [x ({"iiii" "i"
                                            "nnnn" "n"
                                            "mmmm" "m"
                                            "dddd" "d"
                                            "----" "-"} %)]
                                 x
                                 (if (re-matches #"\d+" %)
                                   (s/upper-case (Integer/toHexString
                                                  (Integer/parseInt % 2)))
                                   (str "("
                                        (s/join (map (fn [c] (get {\0 0 \1 1} c "-")) %))
                                        ")")))
                              (s/split (:op op) #" ")))]
     
     (set-comments cs (str (:name op) " [" instr-op "]") (:desc op))
     (doseq [[i mc] (map vector (range) (:slots op))]
       (let [alt (.createAlternative cs (choices (HexLiteral. i)))]
         (try
           (add-all alt statements
                    (-> op
                        (vhdlmc/gen-assigns-check mc)
                        (transform-values transforms)
                        (gen-control-assignments controls)))
           (catch Exception e
             (println "Failed to generate" (:name op) (str "[" instr-op "]") "line" (:line mc))
             (throw e)))))
     (.createAlternative cs (choices Choices/OTHERS))
     cs)])

(defn gen-rom-cases [cond-var line-sig op lines table-inputs controls transforms]
  (let [op-cond (logic/op-to-logic-map op)]
    (map-indexed
     (fn [i line]
       [(func-call-pos
         std-match
         cond-var
         (StringLiteral. (logic/logic-map-to-stdmatch
                          (merge
                           op-cond
                           (logic/str-to-logic-map :s (s/join (repeat 4 \0)))
                           (logic/str-to-logic-map :s (Long/toBinaryString i)))
                          :p 1 :s 4 :i 16)))
        (set-comments (assign line-sig line) (s/join " " [(:name op) "seq" i]))])
     lines)))

(defn key-lookuper [keys]
  (fn [m]
    (map #(get m %) keys)))

(defn best-enum-mapping [all-bits num-literals values]
  (->> (combo/combinations all-bits (log2-ceil num-literals))
       (map
        (fn [bits]
          (let [value-classes
                (map (fn [[v maps]]
                       (let [found-values
                             (frequencies
                              (->> maps
                                   (map #(select-keys % bits))
                                   (filter #(= (count bits) (count %)))))]
                         (if (seq found-values)
                           found-values
                           [[nil 0]])))
                     values)
                possible-assignments
                (apply combo/cartesian-product value-classes)]
            (when (seq possible-assignments)
              (let [[cost mappings]
                    (->> possible-assignments
                         (filter #(= (count values) (count (distinct (map first %)))))
                         (map (fn [x]
                                [(apply + (map second x))
                                 x]))
                         (sort-by first)
                         first)]
                (when cost
                  {:bits bits
                   :cost cost
                   :bit-vals
                   (zipmap (keys values)
                           (map #(when-let [bitmap (first %)]
                                   (map (fn [b]
                                          (bitmap b))
                                        bits)) mappings))}))))))
       (filter identity)
       (sort-by second)
       last))

(defn optimize-enums [transforms control-sigs enum-types maps]
  (let [enum-mapping
        (->> maps
             (map
              (fn [[k values]]
                (let [sig (control-sigs k)
                      t (cond
                         (instance? RecordElement sig) (record-element-type sig)
                         (instance? Signal sig) (.getType sig))]
                  (when (and t (instance? EnumerationType t)
                             #_(= t (:mem-size enum-types)))
                    ;;(println "k ->" (.getType sig) t)
                    (let [num-literals (count (.getLiterals t))
                          all-bits (->> (vals values)
                                        (apply concat)
                                        (mapcat keys)
                                        distinct)]

                      (if-let [{:keys [bits cost bit-vals]}
                               (best-enum-mapping all-bits num-literals values)]
                        (let [val-imps
                              (into {}
                                    (map
                                     (fn [[k b]]
                                       (let [lookup (key-lookuper bits)]
                                         [k
                                          (group-by
                                           #(if (= (lookup %) b)
                                              :combine
                                              :seperate)
                                           (values k))]))
                                     bit-vals))
                              sym (gensym)
                              new-values
                              (assoc (into {}
                                           (map (fn [[k v]]
                                                  [k (:seperate v)])
                                                val-imps))
                                sym (mapcat (comp :combine second) val-imps))

                              calc-costs #(into {}
                                                (map (fn [[k v]]
                                                       [k (apply + (map logic/logic-map-cost v))])
                                                     %))
                              #__ #_(println "BEFORE cost: " (calc-costs values))
                              ;;                  _ (println "BEFORE reduce cost: " (calc-costs
                                        ;                  new-values))
                              ]

                          ;; rerun reductions with the new implicant groups
                          (if (> ((calc-costs new-values) sym)
                                 (apply max (vals (calc-costs values))))
                            (let [new-values (into {}
                                                   (map
                                                    (fn [[k imps]]
                                                      [k (if (seq imps)
                                                           (logic/reduce-implicants imps) imps)])
                                                    new-values))]
                              #_(println "IMPROVE" k (calc-costs new-values))
                              {:control k
                               :sym sym
                               :bits bits
                               :values new-values})
                            #_(println "WORSE" k (calc-costs new-values)))
                          #_(println "BEST:" (get-id t) ":" cost bits bit-vals))
                        #_(println "NO BEST:" (get-id t))))))))
             (filter identity))]
    [transforms
     #_(apply assoc transforms
            (mapcat
             (fn [{:keys [control values sym bits]}]
               (let [sub-transform (or (transforms control) identity)]
                 [control
                  (fn [v]
                    (if (= v sym)
                      (attr-val
                       (Constant.
                        (get-id (enum-types control))
                        std-logic)
                       (DecimalLiteral. 0))
                      (sub-transform v)))]))
             enum-mapping))
     maps
     #_(apply assoc maps
            nil
            (mapcat
             (fn [{:keys [control values]}]
               [control values])
             enum-mapping))]))

(defn- create-reset-vector [record enum-maps addr]
  (let [op-agg
        (named-agg
         ((:instr-plane enum-maps) :system)
         "plane"

         (num-val (std-logic-vector 16) 0x0300)
         "code"

         (num-val (std-logic-vector 8) addr)
         "addr")]
    (apply named-agg
           (apply concat
                  (for [elem (.getElements record)
                        ident (.getIdentifiers elem)]
                    [(case ident
                       "op" op-agg
                       "instr_seq" (StringLiteral. "0001")
                       (zero-val (.getType elem)))
                     (signal ident (.getType elem))])))))

(defn gen-decoder [ops & {:keys [rom-width] :or {rom-width 72}}]
  (let [world (inter/generate-interface ops)
        {:keys [enum-types enum-maps enum-default-value pipelines pipeline-records
                decode-inputs decode-outputs op-record nillable-outputs
                multi-stage-signals
                control-elements control-records]} world
        ops (map #(assoc % :logic-map (logic/op-to-logic-map %)) ops)

        pipeline-signals (apply create-signals (map reverse pipeline-records))

        pipeline-record (record-type "pipeline_t"
                                 [(:ex pipeline-records) :ex1]
                                 [(:ex-stall pipeline-records) :ex1-stall]
                                 [(:wb pipeline-records) :wb1 :wb2 :wb3]
                                 [(:wb-stall pipeline-records)
                                  :wb1-stall :wb2-stall :wb3-stall])

        ex-reset-const (gen-const "STAGE_EX_RESET" (:ex pipeline-records))
        wb-reset-const (gen-const "STAGE_WB_RESET" (:wb pipeline-records))
        ex-stall-reset-const (gen-const "STAGE_EX_STALL_RESET" (:ex-stall pipeline-records))
        wb-stall-reset-const (gen-const "STAGE_WB_STALL_RESET" (:wb-stall pipeline-records))
        pipeline-reset-const (gen-const "PIPELINE_RESET" pipeline-record
                                    (into {} (concat (mapcat
                                                      (fn [i]
                                                        [[(str "wb" i) wb-reset-const]
                                                         [(str "wb" i "_stall") wb-stall-reset-const]])
                                                      [1 2 3])
                                                     [["ex1" ex-reset-const]
                                                      ["ex1_stall" ex-stall-reset-const]])))

        internal-signals (merge pipeline-signals
                                (create-signals [std-logic
                                                 :next-id-stall :dispatch
                                                 :dispatch :delay-jump
                                                 :ilevel-cap
                                                 :event-ack-0
                                                 ;;(with-meta 'debug-o {:key :debug})
                                                 :debug-o
                                                 :mac-stall-sense
                                                 :maskint-next :maskint-o]
                                                [op-record :op]
                                                [pipeline-record :pipeline-c :pipeline-r]))

        table-inputs (merge (select-keys internal-signals [:next-id-stall :op])
                            (select-keys decode-inputs [:t-bcc :clk]))
        table-outputs (merge
                       (select-keys decode-outputs [:slp :debug])
                       (create-signals [std-logic :mac-s-latch])
                       (select-keys internal-signals (keys pipeline-records))
                       (select-keys internal-signals [:dispatch :delay-jump :ilevel-cap
                                                      :event-ack-0 :mac-stall-sense :maskint-next]))

        core-inputs (merge (dissoc decode-inputs :mask-int)
                           (dissoc pipeline-signals :wb :wb-stall)
                           (create-signals [pipeline-record (->KeySig :pipeline-r "p")])
                           (select-keys internal-signals [:dispatch :event-ack-0
                                                          :mac-stall-sense
                                                          :delay-jump :ilevel-cap])
                           (select-keys decode-outputs [:debug])
                           {:maskint-o (:maskint-next internal-signals)})
        core-outputs (merge
                      (select-keys decode-outputs [:event-ack])
                      (create-signals [(std-logic-vector 4) :ilevel]
                                      [std-logic :if-issue :ifadsel :incpc])
                      (select-keys internal-signals
                                   [:op :next-id-stall]))

        predecode-inputs (create-signals [(std-logic-vector 16) :code])
        predecode-outputs (create-signals [(std-logic-vector 8) :addr])
        ;; Some of the decode table and decode output signal names
        ;; don't match for historical reasons. Add the alternates
        ;; here.
        decode-output-elements
        (into control-elements
              (map (fn [[k1 k2]]
                     [k2 (control-elements k1)])

                   ;; decode output name on left, table output/pipeline on right
                   (partition
                    2
                    [:regwr-z :wrreg-z
                     :regwr-w :wrreg-w
                     :maccom1 :mulcom1
                     :maccom2 :mulcom2
                     :macwrmach :wrmach
                     :macwrmacl :wrmacl
                     :macs-latch :mac-s-latch
                     :memsize :mem-size
                     :memaddr-sel :mem-addr-sel
                     :memwdata-sel :mem-wdata-sel
                     :memlock :mem-lock
                     :memissue :ma-issue
                     :memwr :ma-wr
                     :memkeep-cyc :keep-cyc
                     :instrissue :if-issue
                     :instraddr-sel :ifadsel
                     :pcwr-z :wrpc-z
                     :pcwrpr :wrpr-pc
                     :pcinc :incpc
                     :busesx-sel :xbus-sel
                     :busesy-sel :ybus-sel
                     :busesz-sel :zbus-sel
                     :busesimm-val :imm-val
                     :funcaluinx-sel :aluinx-sel
                     :funcaluiny-sel :aluiny-sel
                     :funcalumanip :alumanip
                     :funcshift :shiftfunc
                     :funcarithfunc :arith-func
                     :funcarithci-en :arith-ci-en
                     :funcarithsr :arith-sr-func
                     :funclogic-func :logic-func
                     :funclogic-sr :logic-sr-func
                     :srsel :sr-sel
                     :srt :t-sel
                     :srilevel :ilevel])))
        internal-signal-overrides
        {:debug (:debug-o internal-signals)}
        [pipeline-inputs
         pipeline-outputs]
        (reduce
         (fn [[ins outs] [k sigs]]
           (let [kstr (s/replace (name k) #"-stall" "")
                 sigs (filter keyword? (flatten sigs))]
             [(into ins (for [s sigs]
                          [(if (multi-stage-signals s)
                             (keyword (str kstr "-" (name s)))
                             s)
                           (RecordElement. (pipeline-signals k) (vhdl-name s))]))
              (into outs (for [s sigs]
                           [(if (multi-stage-signals s)
                              (keyword (str kstr "-" (name s)))
                              s)
                            (rec-elem
                             (rec-elem (:pipeline-r internal-signals)
                                       (vhdl-name (get {:ex :ex1 :ex-stall :ex1-stall
                                                        :wb :wb3 :wb-stall :wb3-stall}
                                                       k k)))
                             (vhdl-name s))]))]))
         [{} {}]
         pipelines)

        instr-code (RecordElement. (:op table-inputs) "code")
        instr-plane (RecordElement. (:op table-inputs) "plane")
        instr-addr (RecordElement. (:op table-inputs) "addr")

        immediates
        (let [t (std-logic-vector 32)]
          (into {} (map (fn [[v literal]]
                          [v
                           (assoc
                               (match [v]
                                      [(n :guard number?)]
                                      {:value (num-val t n)
                                       :sort [:n n]}
                                      [[:s width shift]]
                                      (let [sig (signal (s/join "_" ["imms" width shift]) t)]
                                        {:value sig
                                         :signal sig
                                         :sort v})
                                      [[:u width shift]]
                                      {:value
                                       (v-cat (zero-val (std-logic-vector (- 32 width shift)))
                                              (slice-downto instr-code
                                                            (dec width) 0)
                                              (when (not (zero? shift))
                                                (zero-val (std-logic-vector shift))))
                                       :sort v})
                             :literal literal)])
                        (:imm-val enum-maps))))

        plane-num-bits (log2-ceil (count (:instr-plane enum-maps)))
        plane-vector (TypeConversion.
                      StdLogic1164/STD_LOGIC_VECTOR
                      (func-call-pos
                       NumericStd/TO_UNSIGNED
                       (attr-pos
                        (Constant. "instruction_plane_t" std-logic)
                        instr-plane)
                       ;; determine how many bits are
                       ;; needed for all enum literals
                       (DecimalLiteral. plane-num-bits)))
        imm-enum-sig (signal "imm_enum" (:imm-val enum-types))
        mac-busy-enum-sig (signal "mac_busy" (:mac-busy enum-types))
        ;;regnum-sigs (create-signals [(enum-types :regnum) (inter/xy :regnum) (inter/zw :regnum)])
        table-controls (merge pipeline-inputs table-outputs)
        imm-proc
        (-> (ProcessStatement.)
            (add-all sensitivities (:op table-inputs))
            ;; assign extended immediate signals
            (add-all statements
                     (let [sign-extend
                           (fn [sig width shift]
                             [(let [for-stmt
                                    (ForStatement. "i"
                                                   (range-to (+ width shift) 31))]
                                (add-all for-stmt statements
                                         [(assign
                                           (ArrayElement. sig (.getParameter for-stmt))
                                           (ArrayElement. instr-code (dec width)))]))
                              (assign
                               (slice-downto sig (dec (+ width shift)) shift)
                               (slice-downto instr-code (dec width) 0))
                              (for [i (range shift)]
                                (assign (ArrayElement. sig i) StdLogic1164/STD_LOGIC_0))])]
                       (map (fn [[[_ width shift] {signal :signal}]]
                              (set-comments (sign-extend signal width shift)
                                            (str "Sign extend " width " right-most bits"
                                                 (when (not (zero? shift)) (str " shifted by " shift)))))
                            (sort-by first 
                                     (filter #(:signal (second %)) immediates))))))
        regnum-val-fn (fn [x]
                        (or ({:ra (v-cat StdLogic1164/STD_LOGIC_0 (slice-downto instr-code 11 8))
                              :rb (v-cat StdLogic1164/STD_LOGIC_0 (slice-downto instr-code 7 4))} x)
                            (if (number? x)
                              (num-val (:regnum-x table-controls) x))))
        #_regnum-muxes #_(let [regnum-enum (:regnum enum-maps)]
                       (for [[k sig] (sort regnum-sigs)]
                         (apply select-assign (table-controls k) sig
                                (mapcat
                                 (fn [x] [(regnum-vals x) (regnum-enum x)])
                                 [:ra :rb 0 15]))))
        controls
        (merge table-controls #_regnum-sigs {:imm-val imm-enum-sig
                                           :mac-busy mac-busy-enum-sig})
        assign-val-transform
        (merge
         (dissoc enum-maps :regnum :macsel1 :macsel2 :mac-op)
         (zipmap [:regnum-x :regnum-y :regnum-z :regnum-w]
                 (repeat regnum-val-fn))
         (zipmap [:ex-macsel1 :wb-macsel1] (repeat (:macsel1 enum-maps)))
         (zipmap [:ex-macsel2 :wb-macsel2] (repeat (:macsel2 enum-maps)))
         (zipmap [:ex-mulcom2 :wb-mulcom2] (repeat (:mac-op enum-maps)))
         (let [issue-dispatch {true StdLogic1164/STD_LOGIC_1
                               0 StdLogic1164/STD_LOGIC_0
                               :t (:t-bcc table-inputs)
                               :nt (v-not (:t-bcc table-inputs))}]
           {:if-issue issue-dispatch
            :dispatch issue-dispatch
            :ma-issue issue-dispatch})
         (let [bit-flag {true StdLogic1164/STD_LOGIC_1
                         1 StdLogic1164/STD_LOGIC_1
                         false StdLogic1164/STD_LOGIC_0
                         0 StdLogic1164/STD_LOGIC_0}]
           {:arith-ci-en bit-flag})
         {:wrpc-z (fn [x]
                    (get {:t (:t-bcc table-inputs)
                          :nt (v-not (:t-bcc table-inputs))}
                         x x))})

        ;; order ops based for ROM. This adds an :index field to each
        ;; op map which is used to create constants in the package
        rom-ops (rom/reorder-microcode ops :nop)
        ;; extract ROM addresses for system instructions
        system-ops (->> rom-ops
                        (filter #(= 1 (:plane %)))
                        (map (fn [op]
                               [(->> (-> op
                                         :name
                                         s/lower-case
                                         (s/split #" +"))
                                     (filter (comp not zero? count))
                                     (map (fn [x] (get {"instruction" "instr"} x x)))
                                     (map s/upper-case)
                                     (s/join "_"))
                                op]))
                        (into {}))

        decode-entity (-> (Entity. "decode")
                          (add-in-ports (sort-by get-id (vals decode-inputs)))
                          (add-out-ports (sort-by get-id (vals decode-outputs))))

        core-entity (-> (Entity. "decode_core")
                        (add-in-ports (sort-by get-id (vals core-inputs)))
                        (add-out-ports (sort-by get-id (vals core-outputs))))

        table-entity (-> (Entity. "decode_table")
                         (add-in-ports (sort-by get-id (vals table-inputs)))
                         (add-out-ports (sort-by get-id (vals table-outputs))))

        predecode-decl (apply func-dec "predecode_rom_addr"
                              StdLogic1164/STD_LOGIC_VECTOR
                              [(Constant. "code" (std-logic-vector 16))])

        illegal-delay-decl (apply func-dec "check_illegal_delay_slot"
                                  StdLogic1164/STD_LOGIC
                                  [(Constant. "code" (std-logic-vector 16))])
        illegal-instr-decl (apply func-dec "check_illegal_instruction"
                                  StdLogic1164/STD_LOGIC
                                  [(Constant. "code" (std-logic-vector 16))])

        pkg-decl (PackageDeclaration. "decode_pack")
        pkg (add-all pkg-decl declarations
                     ;; remove enums that are defined outside of decode_pkg
                     (map second (sort (dissoc enum-types :alufunc
                                               :arith-func :logic-func
                                               :arith-sr-func :logic-sr-func
                                               :shiftfunc :pc-sel :mac-op :mem-size :alumanip)))
                     op-record
                     (sort-by get-id (vals control-records))
                     (sort-by get-id (vals pipeline-records))
                     pipeline-record
                     (Component. decode-entity)
                     (Component. core-entity)
                     (Component. table-entity)
                     predecode-decl
                     illegal-delay-decl
                     illegal-instr-decl
                     (let [decode-core-record
                           (record-type "decode_core_reg_t"
                                        [std-logic :maskint :delay-slot :id-stall :instr-seq-zero]
                                        [op-record :op]
                                        [(std-logic-vector 4) :ilevel])]
                       [decode-core-record
                        (ConstantDeclaration.
                         (.getVhdlObjects
                          (Constant. "DEC_CORE_RESET"
                                     decode-core-record
                                     (create-reset-vector
                                      decode-core-record
                                      enum-maps
                                      ;; Use 1 to start at second step
                                      ;; in instruction. Not sure why,
                                      ;; but likely to set up decoder
                                      ;; state. TODO: reexamine if
                                      ;; this is necessary.
                                      1))))
                        ;; separate reset contant for ROM based decoder
                        (set-comments
                         (ConstantDeclaration.
                          (.getVhdlObjects
                           (Constant. "DEC_CORE_ROM_RESET"
                                      decode-core-record
                                      (create-reset-vector
                                       decode-core-record
                                       enum-maps
                                       ;; Add + 1 to start at second
                                       ;; step in instruction. Not sure
                                       ;; why, but likely to set up
                                       ;; decoder state. TODO:
                                       ;; reexamine if this is
                                       ;; necessary.
                                       (if-let [op (get system-ops "RESET_CPU")]
                                         (inc (:index op))
                                         (throw (IllegalStateException.
                                                 "cannot determine reset instruction address")))))))
                         "Reset vector specific to the microcode ROM. Uses a different starting addr.")])
                     (let [addr-type (std-logic-vector 8)
                           event-code-type (std-logic-vector 11 8)
                           sys-ops (sort-by first system-ops)
                           system-instr-enum
                           (EnumerationType.
                            "system_instr_t"
                            (into-array (map first sys-ops)))
                           array-type
                           (UnconstrainedArray. "system_instr_addr_array"
                                             addr-type
                                             (into-array [system-instr-enum]))
                           sys-instr-addr-constant
                           (Constant. "system_instr_rom_addrs" array-type
                                      (apply named-agg
                                             (interleave
                                              (map (fn [[name op]]
                                                     (num-val addr-type (:index op)))
                                                   sys-ops)
                                              (.getLiterals system-instr-enum))))
                           instr-code-array
                           (UnconstrainedArray. "system_instr_code_array" event-code-type (into-array [system-instr-enum]))

                           instr-code-constant
                           (Constant. "system_instr_codes" instr-code-array
                                      (apply named-agg
                                             (interleave
                                              (map (partial num-val (std-logic-vector 4)) [2 1 7 0 3 6])
                                              (.getLiterals system-instr-enum))))
                           ;; cpu_event_cmd_t is defined outside the
                           ;; debug vhdl pkg
                           cpu-event-cmd-t (EnumerationType. "cpu_event_cmd_t" (into-array ["INTERRUPT", "ERROR", "BREAK", "RESET_CPU"]))
                           event-code-array
                           (UnconstrainedArray. "system_event_code_array" event-code-type (into-array [cpu-event-cmd-t]))
                           event-code-constant
                           (Constant. "system_event_codes" event-code-array
                                      (apply named-agg
                                             (interleave
                                              (map (partial num-val (std-logic-vector 4)) [0 1 2 3])
                                              (.getLiterals cpu-event-cmd-t))))
                           event-instr-array
                           (UnconstrainedArray. "system_event_instr_array" system-instr-enum (into-array [cpu-event-cmd-t]))
                           event-instr-constant
                           (Constant. "system_event_instrs" event-instr-array
                                      (apply named-agg
                                             (let [lit-pair #(into {} (map (fn [l] [(str l) l]) (.getLiterals %)))
                                                   instr-literals (lit-pair system-instr-enum)
                                                   event-literals (lit-pair cpu-event-cmd-t)]
                                               (mapcat (fn [[i e]]
                                                         [(get instr-literals i) (get event-literals e)])
                                                    [["INTERRUPT" "INTERRUPT"]
                                                     ["ERROR" "ERROR"]
                                                     ["BREAK" "BREAK"]
                                                     ["RESET_CPU" "RESET_CPU"]]))))]
                       [system-instr-enum
                        array-type
                        (ConstantDeclaration. (into-array [sys-instr-addr-constant]))
                        instr-code-array
                        (ConstantDeclaration. (into-array [instr-code-constant]))
                        event-code-array
                        (ConstantDeclaration. (into-array [event-code-constant]))
                        event-instr-array
                        (ConstantDeclaration. (into-array [event-instr-constant]))]))

        {:keys [clk rst slot]} decode-inputs
        gen-compressed-stmts
        (fn [kind ops control-sigs transforms plane-sig instr-sig seq-sig]
          (let [to-bit-cond #(logic/logic-map-to-bit % {:p plane-sig :i instr-sig :s seq-sig})
                ;; determine the default control values 
                default-controls
                (->> (dissoc control-sigs
                             :id :ex :wb
                             :ex-stall :wb-stall
                             :ex-mac-busy :wb-mac-busy)
                     keys
                     (map (fn [k]
                            (when (not (nillable-outputs k))
                                    [k (or (enum-default-value k) 0)])))
                     (filter identity)
                     (into {}))
                mcodes
                (->> ops
                     (mapcat
                      (fn [op]
                        (let [slots (:slots op)
                              num-slots (count slots)
                              seq-bits (- Long/SIZE (Long/numberOfLeadingZeros (dec num-slots)))]
                          (map-indexed (fn [i mc]
                                         (let [cond
                                               (merge (:logic-map op)
                                                      (when (> seq-bits 0)
                                                        (merge (logic/str-to-logic-map :s (apply str (repeat seq-bits \0)))
                                                               (logic/str-to-logic-map :s (Long/toBinaryString i)))
                                                        ))]
                                           {:seq-bits seq-bits
                                            :logic-map cond
                                            :controls (merge default-controls
                                                             (vhdlmc/gen-assign-map op mc))}))
                                       slots)))))

                ;; relax the logic-maps
                #_mcodes
                #_(map
                 #(assoc %1 :logic-map %2)
                 mcodes
                 (logic/relax-logic-maps (map :logic-map mcodes)))]
            #_(println "DEFAULTS" default-controls)
            (case kind
              :reverse
              (->> mcodes
                   (mapcat
                    (fn [m]
                      (for [[k v] (:controls m)]
                        {:key k
                         :value v
                         :logic-map (:logic-map m)})))
                   (group-by (juxt :key :value))
                   (map
                    (fn [[ks maps]]
                      [ks
                       (let [seq-width
                             (count
                              (->> (map :logic-map maps)
                                   (map #(select-keys % (for [i (range 4)]
                                                          [:s i])))
                                   (apply merge)))
                             leading-zeros
                             (into {} (for [i (range seq-width)]
                                        [[:s i] 0]))]
                         ;; add leading zeros to instr-seq
                         ;; conditions so they are more
                         ;; restrictive (while still being
                         ;; correct) to allow more reductions
                         (->> (map :logic-map maps)
                              ;; leading zeros may cause more delay...
                              #_(map (partial merge leading-zeros))
                              logic/reduce-implicants))]))
                   ;; build nested map structure
                   ;; key -> value -> [implicants]
                   (reduce
                    (fn [m [ks implicants]]
                      (assoc-in m ks implicants))
                    {})
                   (optimize-enums transforms control-sigs enum-types)
                   ((fn [[transforms maps]]
                      
                      (let [maps
                            (into {}
                                  (map
                                   (fn [[k values]]
                                     [k
                                      (sort-by #(apply + (map count (second %))) values)])
                                   maps))
                            ;; compute common implicants and cache in new signals
                            imp-bit-cache
                            (into {}
                                  (map-indexed
                                   (fn [i [imp cnt]]
                                     (let [sig (signal (str "imp_bit_" i) std-logic)]
                                       [imp sig]))
                                   (filter
                                    ;;identity                                    
                                    #(<= 2 (second %))
                                    (frequencies
                                     (apply concat
                                            (mapcat #(map second (butlast %)) (vals maps)))))))
                            to-bit #(or (imp-bit-cache %)
                                        (to-bit-cond %))
                            sig-assigns
                            (map-indexed
                             (fn [i [key values]]
                               (let [sig (control-sigs key)
                                     values
                                     (map
                                      (fn [[val conds]]
                                        (let [val ((or (get transforms key) identity) val)
                                              val (if (number? val)
                                                    (num-val sig val)
                                                    val)]
                                          #_(println "VAL" val)
                                          [val conds]))
                                      values)]
                                 (if (and (= std-logic (type-of sig))
                                          (= 2 (count values)))
                                   ;; a single std-logic can be directly assigned
                                   (let [[val conds] (first values)
                                         exp (apply v-or (map to-bit conds))]
                                     [nil
                                      (cond-assign sig
                                                   (if (= val StdLogic1164/STD_LOGIC_1)
                                                     exp
                                                     (v-not exp)))])
                                   (let [conditions 
                                         (map
                                          (fn [[v conds]]
                                            (apply v-or (map to-bit conds)))
                                          (butlast values))]
                                     #_(apply println (name key) "=" (mapcat (fn [[v conds]]
                                                                         [v (count conds)])
                                                                             values))
                                     (case (count values)
                                       1
                                       [nil (cond-assign sig (first (first values)))]
                                       2
                                       ;; two values don't need an
                                       ;; intermediate signal for
                                       ;; condition
                                       (let [[v1 v2] (map first values)]
                                         [nil
                                          (cond-assign
                                           sig
                                           v1 (v= (first conditions) StdLogic1164/STD_LOGIC_1)
                                           v2)])

                                       ;; more than 2
                                       (let [temp-sig (signal (str "cond" i)
                                                              (std-logic-vector (dec (count values))))]
                                         [temp-sig
                                          (cond-assign
                                           temp-sig
                                           (apply v-cat conditions))
                                          (apply
                                           select-assign
                                           sig
                                           temp-sig
                                           (concat
                                            (butlast
                                             (mapcat
                                              (fn [i v]
                                                [v
                                                 (StringLiteral.
                                                  (str (s/join (repeat i "0")) \1
                                                       (s/join (repeat (- (count values) i 2) "0"))))])
                                              (range)
                                              (map first values)))
                                            [Choices/OTHERS]))]))))))
                             maps)]

                        [(concat (vals imp-bit-cache)
                                 (filter identity (map first sig-assigns)))
                         (concat
                          (map (fn [[imp sig]] (cond-assign sig (to-bit-cond imp))) imp-bit-cache)
                          (mapcat rest sig-assigns))])))))))
        arch (-> (set-comments (Architecture. "arch" decode-entity)
                        #_"Signals in old decode.v that were always 0"
                        #_(map
                         #(str "- " %)
                         ["WB_MACSEL1_0"
                          "WB_MACSEL2_0"
                          "EX_RDMACH_X"
                          "EX_RDMACL_X"
                          "EX_WRMADW_X"
                          "WB_MULCOM2_4"
                          "WB_MULCOM2_5"
                          "EX_RDSR_X"
                          "EX_RDPR_X"]))
                 (add-declarations
                  (sort-by get-id (vals internal-signals))
                  [ex-reset-const wb-reset-const
                   ex-stall-reset-const wb-stall-reset-const
                   pipeline-reset-const])
                 (add-all statements
                          (cond-assign (:maskint-o internal-signals)
                                       (v-or (:mask-int decode-inputs)
                                             (:maskint-next internal-signals)))
                          (cond-assign (:debug decode-outputs) (:debug-o internal-signals))
                          (instantiate-component "core" core-entity
                                                 (let [all-sigs (merge decode-outputs decode-inputs
                                                                       table-outputs internal-signals
                                                                       decode-output-elements
                                                                       internal-signal-overrides)]
                                                   (map (fn [[k sig]]
                                                          [(get-id sig) (all-sigs k)])
                                                        (concat (sort core-inputs)
                                                                (sort core-outputs)))))
                          (instantiate-component "table" table-entity
                                                 (let [all-sigs (merge table-outputs table-inputs
                                                                       decode-output-elements
                                                                       internal-signal-overrides)]
                                                   (map (fn [[k sig]]
                                                          [(get-id sig) (all-sigs k)])
                                                        (concat (sort table-inputs)
                                                                (sort table-outputs)))))
                          (let [this (Variable. "pipe" pipeline-record)
                                this-r (:pipeline-r internal-signals)
                                next-stall (v= (:next-id-stall internal-signals) 1)]
                            (set-comments
                             (-> (ProcessStatement.)
                                 (add-declarations this)
                                 (add-all sensitivities
                                          (sort-by get-id
                                                   (vals (select-keys table-outputs
                                                                      [:ex :ex-stall :wb :wb-stall])))
                                          (:next-id-stall internal-signals)
                                          this-r slot)
                                 (add-all statements
                                          (varassign this this-r)
                                          (if-stmt
                                           (v= slot 1)
                                           [(varassign (rec-elem this "wb3")
                                                       (rec-elem this "wb2"))
                                            (varassign (rec-elem this "wb2")
                                                       (rec-elem this "wb1"))
                                            (varassign (rec-elem this "wb1") (:wb table-outputs))
                                            (varassign (rec-elem this "ex1") (:ex table-outputs))
                                            (varassign (rec-elem this "wb3_stall")
                                                       (rec-elem this "wb2_stall"))
                                            (varassign (rec-elem this "wb2_stall")
                                                       (rec-elem this "wb1_stall"))
                                            (if-stmt next-stall
                                                     [(varassign
                                                       (rec-elem this "ex1_stall")
                                                       ex-stall-reset-const)
                                                      (varassign
                                                       (rec-elem this "wb1_stall")
                                                       wb-stall-reset-const)]
                                                     [(varassign
                                                       (rec-elem this "ex1_stall")
                                                       (:ex-stall table-outputs))
                                                      (varassign
                                                       (rec-elem this "wb1_stall")
                                                       (:wb-stall table-outputs))])])
                                          (assign (:pipeline-c internal-signals) this)))
                             "pipeline controls signals"))
                          (-> (ProcessStatement.)
                              (add-all sensitivities clk rst)
                              (add-all statements
                                       (if-stmt (v= rst 1)
                                                [(assign (:pipeline-r internal-signals)
                                                         pipeline-reset-const)]
                                                (v-and (v= clk 1) (attr-event clk))
                                                [(assign (:pipeline-r internal-signals)
                                                         (:pipeline-c internal-signals))]))))
                 ;; assign aliases to outputs
                 (add-all statements
                          (let [multi-stage-keys (sort multi-stage-signals)

                                single-ex-stall-keys
                                (sort (filter (complement multi-stage-signals)
                                              (flatten (map rest (:ex-stall pipelines)))))
                                single-wb-stall-keys
                                (sort (filter (complement multi-stage-signals)
                                              (flatten (map rest (:wb-stall pipelines)))))
                                wb-ex-keys
                                (sort
                                 (flatten
                                  (map rest
                                       (apply concat
                                              (vals (select-keys pipelines [:wb :ex]))))))]
                            (let [decode-outputs (merge decode-outputs decode-output-elements)]
                              [(set-comments
                                (for [k (concat wb-ex-keys single-ex-stall-keys single-wb-stall-keys)
                                      :let [sig (decode-outputs k)]
                                      :when (and sig (not (core-outputs k)) (not= :sr-sel k))]
                                  (cond-assign sig (pipeline-outputs k)))
                                "assign outputs")
                               (set-comments
                                (for [k multi-stage-keys
                                      :let [sig (decode-outputs k)]
                                      :when (and sig (not (#{:macsel1 :macsel2 :mulcom2 :maccom2} k)))]
                                  (cond-assign sig
                                               (apply v-or
                                                      (map pipeline-outputs
                                                           (inter/prefix-keys ["ex-" "wb-"] k)))))
                                "assign combined outputs")
                               (let [ex-mulcom2-valid (vnot= (pipeline-outputs :ex-mulcom2)
                                                             (get-in enum-maps [:mac-op :nop]))]
                                 [(cond-assign (decode-outputs :macsel1)
                                               (pipeline-outputs :ex-macsel1)
                                               (v= (v-or
                                                    (pipeline-outputs :ex-mulcom1)
                                                    (pipeline-outputs :ex-wrmach))
                                                   StdLogic1164/STD_LOGIC_1)
                                               (pipeline-outputs :wb-macsel1))
                                  (cond-assign (decode-outputs :macsel2)
                                               (pipeline-outputs :ex-macsel2)
                                               (v-or
                                                ex-mulcom2-valid
                                                (v= (pipeline-outputs :ex-wrmacl)
                                                    StdLogic1164/STD_LOGIC_1))
                                               (pipeline-outputs :wb-macsel2))
                                  (cond-assign (decode-outputs :mulcom2)
                                               (pipeline-outputs :ex-mulcom2)
                                               ex-mulcom2-valid
                                               (pipeline-outputs :wb-mulcom2))
                                  (cond-assign (decode-outputs :srsel)
                                               (get-in enum-maps [:sr-sel :wbus])
                                               (v= (pipeline-outputs :wrsr-w)
                                                   StdLogic1164/STD_LOGIC_1)
                                               (pipeline-outputs :sr-sel))])]))))

        logic-table
        (-> (Architecture. "simple_logic" table-entity)
            (add-declarations
             imm-enum-sig mac-busy-enum-sig
             (sort-by get-id
                      (filter identity
                              (map :signal (vals immediates)))))
            (add-all statements
                     (set-comments
                      (apply
                       select-assign
                       (table-controls :imm-val)
                       imm-enum-sig
                       (mapcat
                        (fn [[_ {:keys [literal value]}]]
                          [value literal])
                        (sort-by #(:sort (second %)) immediates)))
                      "Immediate value mux")
                     (set-comments imm-proc "Sign extend parts of opcode")
                     (set-comments
                      (let [enum (:mac-busy enum-maps)
                            not-stall (Not. (:next-id-stall internal-signals))]
                        [(select-assign (table-controls :ex-mac-busy)
                                        mac-busy-enum-sig
                                        0 (enum :nop)
                                        not-stall (enum [:ex :not-stall])
                                        0 (enum [:wb :not-stall])
                                        1 (enum [:ex :busy])
                                        0 (enum [:wb :busy]))
                         (select-assign (table-controls :wb-mac-busy)
                                        mac-busy-enum-sig
                                        0 (enum :nop)
                                        0 (enum [:ex :not-stall])
                                        not-stall (enum [:wb :not-stall])
                                        0 (enum [:ex :busy])
                                        1 (enum [:wb :busy]))])
                      "Mac busy muxes")
                     (let [cond-var (Variable. "cond" (std-logic-vector (+ plane-num-bits 16)))
                           cases (map #(gen-op-case cond-var % (slice-downto instr-addr 3 0) controls
                                                    assign-val-transform)
                                      ops)]
                       (-> (ProcessStatement.)
                           (add-all sensitivities
                                    (vals (dissoc table-inputs :next-id-stall :clk)))
                           (add-declarations cond-var)
                           (add-all statements
                                    (varassign cond-var
                                               (v-cat plane-vector instr-code))
                                    (set-comments (map assign-zero
                                                       (vals (dissoc table-outputs :ex :wb)))
                                                  "zero outputs by default"))
                           ;; Zero out all of ex and wb pipeline outputs
                           ;; except for signals set outside LUT:
                           ;; regnum-x/y/z/w, imm_val, mac_busy
                           (add-all statements
                                    (let [rec-set #{(:ex pipeline-signals) (:wb pipeline-signals)}]
                                      (map assign-zero
                                           (->> table-controls
                                                vals
                                                (filter #(instance? RecordElement %))
                                                (filter #(rec-set (.getPrefix %)))
                                                (filter #(and
                                                          #_(not (.startsWith (.getElement %) "regnum"))
                                                          (not (#{"imm_val" "mac_busy"}
                                                                (.getElement %))))))))
                                    #_(map assign-zero (vals regnum-sigs))
                                    (assign-zero imm-enum-sig)
                                    (assign-zero mac-busy-enum-sig)
                                    (when (seq cases)
                                      (set-comments
                                       (apply if-stmt (apply concat cases))
                                       "set control signals for each opcode")))))))

        compressed-table
        (let [p-sig (signal "p" (std-logic-vector plane-num-bits))
              [compressed-sigs compressed-stmts]
              (gen-compressed-stmts
               :reverse
               ops
               ;; override some of the enums to assign the value
               ;; directly
               (assoc table-controls :mac-busy mac-busy-enum-sig)
               (assoc
                   assign-val-transform
                 :imm-val
                 (into {}
                       (for [[k v] (:imm-val enum-maps)]
                         (let [imm (immediates k)]
                           [k (or (:signal imm) (:value imm))]))))
               p-sig
               instr-code
               instr-addr)]
          (-> (Architecture. "reverse_logic" table-entity)
              (add-declarations
               mac-busy-enum-sig
               (sort-by get-id
                        (filter identity
                                (map :signal (vals immediates))))
               (sort-by get-id compressed-sigs)
               p-sig)
              (add-all statements
                       (set-comments imm-proc "Sign extend parts of opcode")
                       (set-comments
                        (let [enum (:mac-busy enum-maps)
                              not-stall (Not. (:next-id-stall internal-signals))]
                          [(select-assign (table-controls :ex-mac-busy)
                                          mac-busy-enum-sig
                                          not-stall (enum [:ex :not-stall])
                                          1 (enum [:ex :busy])
                                          0)
                           (select-assign (table-controls :wb-mac-busy)
                                          mac-busy-enum-sig
                                          not-stall (enum [:wb :not-stall])
                                          1 (enum [:wb :busy])
                                          0)])
                        "Mac busy muxes")
                       (cond-assign p-sig
                                    0 (v= instr-plane (Constant. "NORMAL_INSTR" std-logic))
                                    1)
                       compressed-stmts)))

        [line-sig-type line-encoder line-decoder]
        (rom/line-encoder (for [op ops
                                slot (:slots op)]
                            (vhdlmc/gen-assign-map op slot))
                          nillable-outputs
                          rom-width)
        ops (map
             (fn [op]
               (assoc op
                 :rom-lines
                 (->> (:slots op)
                      (map vhdlmc/gen-assign-map (repeat op))
                      (mapv line-encoder))))
             rom-ops)
        rom-lines (reduce
                   (fn [all-lines {:keys [index rom-lines]}]
                     (apply assoc all-lines
                            (mapcat (fn [i line]
                                      [i line])
                                    (range index (+ index (count rom-lines)))
                                    rom-lines)))
                   {} ops)
        rom-arch
        (let [line-sig (signal "line" line-sig-type)
              addr-sig (signal "addr" (std-logic-vector 8))
              mem-array (ConstrainedArray. "mem" line-sig-type
                                           (into-array [(range-to 0 255)]))
              zero-line (zero-val line-sig-type)
              index-to-name (into {} (map (fn [op] [(+ (:index op) (count (:slots op)))
                                                   (:name op)]) ops))
              microcode-constant (Constant. "microcode_rom" mem-array
                                            (apply named-agg
                                             (mapcat
                                              (fn [i]
                                                [(or (rom-lines i) zero-line)
                                                 (apply set-comments (DecimalLiteral. i)
                                                        (if-let [c (get index-to-name i)]
                                                          [c]
                                                          nil))])
                                              (range 256))))]
          (-> (Architecture. "rom" table-entity)
              (add-declarations
               line-sig addr-sig mac-busy-enum-sig
               (sort-by get-id
                        (filter identity
                                (map :signal (vals immediates))))
               mem-array
               (ConstantDeclaration. (into-array [microcode-constant])))
              (add-all statements
                       (let [vlk (:clk table-inputs)]
                         (set-comments
                          (-> (ProcessStatement.)
                              (add-all sensitivities clk (:op table-inputs))
                              (add-all statements
                                       (if-stmt (v-and (v= clk 0) (attr-event clk))
                                                (assign line-sig
                                                        (.getArrayElement
                                                         microcode-constant
                                                         (func-call-pos
                                                          NumericStd/TO_INTEGER
                                                          (TypeConversion.
                                                           NumericStd/UNSIGNED
                                                           instr-addr)))))))
                          "Read microcode line on falling edge of"
                          "clock. Needs to be clocked so that xilinx"
                          "uses a RAM, and needs to be falling edge to"
                          "allow the ROM address to be computed."))
                       (set-comments imm-proc "Sign extend parts of opcode")
                       (let [enum (:mac-busy enum-maps)
                             not-stall (Not. (:next-id-stall internal-signals))]
                         [(select-assign (table-controls :ex-mac-busy)
                                         mac-busy-enum-sig
                                         1 (enum [:ex :busy])
                                         not-stall (enum [:ex :not-stall])
                                         0)
                          (select-assign (table-controls :wb-mac-busy)
                                         mac-busy-enum-sig
                                         1 (enum [:wb :busy])
                                         not-stall (enum [:wb :not-stall])
                                         0)])
                       (line-decoder line-sig
                                     (assoc table-controls :mac-busy mac-busy-enum-sig)
                                     (assoc
                                         assign-val-transform
                                       :imm-val
                                       (into {}
                                             (for [[k v] (:imm-val enum-maps)]
                                               (let [imm (immediates k)]
                                                 [k (or (:signal imm) (:value imm))]))))))))]

    ;; build files
    (let [use-pkgs (for [pkg ["cpu2j0_components_pack" "mult_pkg"]]
                     (UseClause. (into-array [(str "work." pkg ".all")])))
          cpu-pkg (UseClause. (into-array [(str "work.cpu2j0_pack.all")]))
          common-hdrs (concat
                       [(LibraryClause. (into-array ["ieee"]))
                        StdLogic1164/USE_CLAUSE
                        NumericStd/USE_CLAUSE
                        (UseClause. (into-array [(str "work." (get-id pkg) ".all")]))]
                       use-pkgs)

          hdr-comment ["******************************************************************"
                       "******************************************************************"
                       "******************************************************************"
                       "This file is generated. Changing this file directly is probably"
                       "not what you want to do. Any changes will be overwritten next time"
                       "the generator is run."
                       "******************************************************************"
                       "******************************************************************"
                       "******************************************************************"]]
      (apply set-comments common-hdrs hdr-comment)
      {:decode
       (add-all (VhdlFile.) elements
                common-hdrs
                cpu-pkg
                decode-entity
                arch)
       :decode-table
       (add-all (VhdlFile.) elements
                common-hdrs
                table-entity)
       :decode-simple
       (add-all (VhdlFile.) elements
                (apply set-comments logic-table hdr-comment))
       :decode-reverse
       (add-all (VhdlFile.) elements
                (apply set-comments compressed-table hdr-comment))
       :decode-rom
       (add-all (VhdlFile.) elements
        (apply set-comments rom-arch hdr-comment))
       :decode-body
       (add-all (VhdlFile.) elements
                common-hdrs
                (-> (PackageBody. pkg-decl)
                        (add-declarations
                         (util/gen-predecode-fn ops predecode-decl)
                         (util/gen-illegal-slot-fn ops illegal-delay-decl)
                         (util/gen-illegal-instr-fn ops illegal-instr-decl))))
       :test-decode-pkg
       (let [pkg-decl (PackageDeclaration. "test_decode_pkg")]
         (add-all (VhdlFile.) elements
                  common-hdrs
                  (PackageBody. pkg-decl)))
       :pkg
       (add-all (VhdlFile.) elements
                (apply set-comments (LibraryClause. (into-array ["ieee"])) hdr-comment)
                StdLogic1164/USE_CLAUSE
                use-pkgs
                cpu-pkg
                pkg)
       :print-op (genc/gen-op-printer ops)})))
