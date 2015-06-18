(ns cpugen.interface
  (:require [clojure.string :as s]
            [clojure.set :as set]
            [clojure.math.combinatorics :as combo]
            [cpugen
             [vhdlmicrocode :as vhdlmc]
             [rom :as rom]
             [logic :as logic]])
  (:use cpugen.vmagic
        [clojure.core.match :only (match)])
  (:import
   clojure.lang.Keyword
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
    IfStatement]
   [de.upb.hni.vmagic.expression
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
    SignalDeclaration
    ConstantDeclaration
    VariableDeclaration
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

(defn control-signals [type & names]
  (into {}
        (for [n names]
          (if (string? n)
            [(keyword (clj-name n)) (Signal. n type)]
            [(first n) (Signal. (second n) type)]))))

(defn prefix-keys [startings & keys]
  (mapcat (fn [k] (map #(keyword (str % (name k))) startings)) keys))

(defn suffix-keys [endings & keys]
  (mapcat (fn [k] (map #(keyword (str (name k) %)) endings)) keys))

(defn xy [& keys]
  (apply suffix-keys ["-x" "-y"] keys))

(defn zw [& keys]
  (apply suffix-keys ["-z" "-w"] keys))

(defn rw [& keys]
  (concat
   (apply xy (apply prefix-keys ["rd"] keys))
   (apply zw (apply prefix-keys ["wr"] keys))))

(defn- find-multi-state-signals [pipelines]
  ;; count instances of keys in each pipeline record to determine
  ;; signals that are in both EX and WB.
  (->>
   (flatten (vals pipelines))
   (filter keyword?)
   (frequencies)
   (filter (fn [[k v]] (> v 1)))
   (keys)
   (into #{})))

(defn generate-interface [ops]
  (let [is-const (fn [x] (if (number? x) x
                            (match [x]
                                   [[(:or :u :s) (n :guard number?) (m :guard number?)]] x)))
        all-imms (->> (mapcat :slots ops)
                  (mapcat (juxt :aluy :x :y))
                  (map is-const)
                  (filter identity)
                  distinct)
        imm-enum
        (concat
         (when (some #{0} all-imms) [[0 "IMM_ZERO"]])
         (map (fn [x] [x (str "IMM_P" x)]) (sort (filter #(and (number? %) (pos? %)) all-imms)))
         (map (fn [x] [x (str "IMM_N" (- x))]) (sort (filter #(and (number? %) (neg? %)) all-imms)))
         (map (fn [[sign width shift]] [[sign width shift]
                                       (s/join "_" ["IMM" (s/upper-case (name sign)) width shift])])
              (filter (complement number?) all-imms)))        
        enums
        (let [sel (fn [k] [k (str "SEL_" (s/upper-case (vhdl-name k)))])]
          [[:arith-func "arith_func_t" "ADD" "SUB"]
           [:arith-sr-func "arith_sr_func_t"
            "ZERO"
            "OVERUNDERFLOW"
            [:u>= "UGRTER_EQ"] [:s>= "SGRTER_EQ"]
            [:u> "UGRTER"] [:s> "SGRTER"]
            "DIV0S" "DIV1"]
           [:logic-func "logic_func_t"
            [:not "LOGIC_NOT"]
            [:and "LOGIC_AND"]
            [:or "LOGIC_OR"]
            [:xor "LOGIC_XOR"]]
           [:logic-sr-func "logic_sr_func_t"
            "ZERO" [:b= "BYTE_EQ"]]
           (apply vector :sr-sel "sr_sel_t"
                  (map sel [:prev :wbus :zbus :div0u :arith :logic :int-mask :set-t]))
           (apply vector :t-sel "t_sel_t"
                  (map sel [:clear :set :shift :carry]))
           [:alumanip "alumanip_t"
            [[:swap :b] "SWAP_BYTE"]
            [[:swap :w] "SWAP_WORD"]
            [[:ext :ub] "EXTEND_UBYTE"]
            [[:ext :uw] "EXTEND_UWORD"]
            [[:ext :sb] "EXTEND_SBYTE"]
            [[:ext :sw] "EXTEND_SWORD"]
            [:xtract "EXTRACT"]
            [:bit7 "SET_BIT_7"]]
           (apply vector :aluinx-sel "aluinx_sel_t" (map sel [:xbus :fc :rotcl :zero]))
           (apply vector :aluiny-sel "aluiny_sel_t" (map sel [:ybus :imm :r0]))
           (apply vector :macsel1 "macin1_sel_t" (map sel [:xbus :zbus :wbus]))
           (apply vector :macsel2 "macin2_sel_t" (map sel [:ybus :zbus :wbus]))
           (apply vector :xbus-sel "xbus_sel_t"
                  (map sel [:imm :reg #_:mach #_:macl :pc #_:sr]))
           (apply vector :ybus-sel "ybus_sel_t"
                  (map sel [:imm :reg :mach :macl :pc :sr]))
           (apply vector :zbus-sel "zbus_sel_t" (map sel [:arith :logic :shift :manip :ybus :wbus]))
           [:shiftfunc "shiftfunc_t" "LOGIC" "ARITH" "ROTATE" [:rotatec "ROTC"]]
           [:regnum "reg_sel_t" [0 "SEL_R0"] [15 "SEL_R15"] [:ra "SEL_RA"] [:rb "SEL_RB"]]
           (apply vector :mem-addr-sel "mem_addr_sel_t" (map sel [:xbus :ybus :zbus]))
           (apply vector :mem-wdata-sel "mem_wdata_sel_t" (map sel [:zbus #_:xbus :ybus]))
           [:mem-size "mem_size_t" "BYTE" "WORD" "LONG"]
           (apply vector :pc-sel "pc_sel_t" (map sel [:nop :wbus :xbus :inc]))

           (apply vector :imm-val "immval_t" imm-enum)
           [:mac-busy "mac_busy_t" [:nop "NOT_BUSY"]
            [[:ex :not-stall] "EX_NOT_STALL"]
            [[:wb :not-stall] "WB_NOT_STALL"]
            [[:ex :busy] "EX_BUSY"]
            [[:wb :busy] "WB_BUSY"]]
           [:instr-plane "instruction_plane_t"
            [:normal "NORMAL_INSTR"] [:system "SYSTEM_INSTR"]]
           [:mac-op "mult_state_t" "NOP" "DMULSL" "DMULUL" "MACL"
                                   "MACW" "MULL" "MULSW" "MULUW"]
           [:cpu-decode-type "cpu_decode_type_t"
            "SIMPLE" "REVERSE" "MICROCODE"]])

        enum-types
        (into {} (for [[key name & vals] enums]
                   [key (EnumerationType.
                         name
                         (into-array (map #(if (string? %) % (second %)) vals)))]))
        enum-maps
        (reduce (fn [m [key name & vals]]
                  (let [enum (enum-types key)
                        literals (into {} (map #(vector (str %) %) (.getLiterals enum)))]
                    (assoc m key
                           (into {} (for [v vals]
                                      (if (string? v)
                                        [(keyword (clj-name v)) (get literals v)]
                                        ;; v is a [key name] pair
                                        [(first v) (get literals (second v))]))))))
                {} enums)
        enum-default-value
        (->> enums
             (map (fn [[key name & vals]]
                          [key
                           (let [v (first vals)]
                             (if (string? v)
                               (keyword (clj-name v))
                               (first v)))]))
             (into {}))
        enum-default-value
        (merge enum-default-value
               (zipmap [:ex-macsel1 :wb-macsel1] (repeat (:macsel1 enum-default-value)))
               (zipmap [:ex-macsel2 :wb-macsel2] (repeat (:macsel2 enum-default-value)))
               (zipmap [:ex-mulcom2 :wb-mulcom2] (repeat (:mac-op enum-default-value))))
        regnum-type (ConstrainedArray. "regnum_t" std-logic [(range-downto 4 0)])
        control-lines
        {:func [{:alu [[(:alumanip enum-types) :manip]
                       [(:aluinx-sel enum-types) :inx-sel]
                       [(:aluiny-sel enum-types) :iny-sel]]}
                [(:shiftfunc enum-types) :shift]
                {:arith [[(:arith-func enum-types) :func]
                         [std-logic :ci-en]
                         [(:arith-sr-func enum-types) :sr]]}
                [(:logic-func enum-types) :logic-func]
                [(:logic-sr-func enum-types) :logic-sr]]
         :reg [[regnum-type (xy :num) (zw :num)]
               [std-logic (zw :wr)]]
         :sr [[(:sr-sel enum-types) :sel]
               [(:t-sel enum-types) :t]
               [(std-logic-vector 4) :ilevel]]
         :mac [[std-logic :com1 :wrmach :wrmacl :s-latch]
               [(:macsel1 enum-types) :sel1]
               [(:macsel2 enum-types) :sel2]
               [(:mac-op enum-types) :com2]]
         :mem [[std-logic :issue :wr :lock]
               [(:mem-size enum-types) :size]
               [(:mem-addr-sel enum-types) :addr-sel]
               [(:mem-wdata-sel enum-types) :wdata-sel]]
         :instr [[std-logic :issue :addr-sel]]
         :pc [[std-logic :wr-z :wrpr :inc]]
         :buses [[(:xbus-sel enum-types) :x-sel]
                 [(:ybus-sel enum-types) :y-sel]
                 [(:zbus-sel enum-types) :z-sel]
                 [(std-logic-vector 32) :imm-val]]}
        [control-records control-elements]
        (letfn [(add-recs [recs elems rec-name fields sig prefix]
                  (let [record (RecordType. (str (vhdl-name rec-name) "_ctrl_t"))
                        recs (assoc recs rec-name record)
                        [sig prefix]
                        (if sig
                          ;; sub record, append to path
                          [(rec-elem sig (vhdl-name rec-name)) (str prefix (name rec-name))]
                          ;; top level record 
                          [(Signal. (vhdl-name rec-name) record) (name rec-name)])]
                    (reduce
                     (fn [[recs elems] field]
                       (cond
                        ;; create fields with the
                        ;; given types
                        (vector? field)
                        (do
                          (let [[t & sigs] field
                                elems
                                (reduce
                                 (fn [elems s]
                                   (.createElement record
                                                   t (into-array [(vhdl-name s)]))
                                   (assoc elems
                                     (keyword (str prefix (name s)))
                                     (rec-elem sig (vhdl-name s))))
                                 elems
                                 (flatten sigs))]
                            [recs elems]))
                        ;; map is a new record
                        ;; definition. recurse to
                        ;; create it. Assume only
                        ;; one k,v pair in map
                        (map? field)
                        (let [[rec-name fields] (first field)
                              [recs elems] (add-recs recs elems rec-name fields sig prefix)]
                          (.createElement record
                                          (recs rec-name)
                                          (into-array [(vhdl-name rec-name)]))
                          [recs elems])
                        :else (throw (IllegalArgumentException.
                                      (str "invalid field " field)))))
                     [recs elems] fields)))]
          (reduce
           (fn [[recs elems] [rec-name fields]]
             (add-recs recs elems rec-name fields nil ""))
           [{} {}]
           control-lines))
        pipelines
        {:id
         [[std-logic :incpc :if-issue :ifadsel]]

         :ex
         [[(std-logic-vector 32) :imm-val]
          [(:xbus-sel enum-types) :xbus-sel]
          [(:ybus-sel enum-types) :ybus-sel]
          [regnum-type (conj (xy :regnum) :regnum-z) ]
          [(:alumanip enum-types) :alumanip]
          [(:aluinx-sel enum-types) :aluinx-sel]
          [(:aluiny-sel enum-types) :aluiny-sel]
          [(:arith-func enum-types) :arith-func]
          [std-logic :arith-ci-en]
          [(:arith-sr-func enum-types) :arith-sr-func]
          [(:logic-func enum-types) :logic-func]
          [(:logic-sr-func enum-types) :logic-sr-func]
          [std-logic :mac-busy :ma-wr :mem-lock]
          [(:mem-size enum-types) :mem-size]]
         :ex-stall
         [[std-logic
           :wrpc-z
           :wrsr-z
           :ma-issue
           :wrpr-pc]
          [(:zbus-sel enum-types) :zbus-sel]
          [(:sr-sel enum-types) :sr-sel]
          [(:t-sel enum-types) :t-sel]
          [(:mem-addr-sel enum-types) :mem-addr-sel]
          [(:mem-wdata-sel enum-types) :mem-wdata-sel]
          [std-logic
           :wrreg-z
           [:wrmach :wrmacl]]
          [(:shiftfunc enum-types) :shiftfunc]
          [std-logic :mulcom1]
          [(:mac-op enum-types) :mulcom2]
          [(:macsel1 enum-types) :macsel1]
          [(:macsel2 enum-types) :macsel2]]

         :wb
         [[regnum-type :regnum-w]
          [std-logic :mac-busy]]
         :wb-stall
         [[std-logic :mulcom1 [:wrmach :wrmacl]
           [:wrreg-w :wrsr-w]]
          [(:macsel1 enum-types) :macsel1]
          [(:macsel2 enum-types) :macsel2]
          [(:mac-op enum-types) :mulcom2]]}]

    {:enum-types enum-types
     :enum-maps enum-maps
     :enum-default-value enum-default-value
     :op-record (record-type "operation_t"
                             [(:instr-plane enum-types) :plane]
                             [(std-logic-vector 16) :code]
                             [(std-logic-vector 8) :addr])
     :control-elements control-elements
     :control-records control-records
     :pipelines pipelines
     :pipeline-records (into {}
                             (for [[k sigs] pipelines]
                               (let [record (RecordType. (str "pipeline_" (vhdl-name k) "_t"))]
                                 (doseq [[t & sigs] sigs
                                         s sigs]
                                   (.createElement record
                                                   t (into-array
                                                      (if (keyword? s)
                                                        [(vhdl-name s)]
                                                        (map vhdl-name (flatten s))))))
                                 [k record])))
     :decode-inputs (create-signals [std-logic :clk :rst :slot :if-stall :mac-busy :t-bcc :enter-debug :mask-int]
                                    [(std-logic-vector 16) :if-dr]
                                    [std-logic :illegal-instr :illegal-delay-slot]
                                    [(RecordType. "cpu_event_i_t") :event-i]
                                    [(std-logic-vector 4) :ibit])
     :decode-outputs (create-signals
                      [(:reg control-records) :reg]
                      [(:func control-records) :func]
                      [(:sr control-records) :sr]
                      [(:mac control-records) :mac]
                      [(:mem control-records) :mem]
                      [(:instr control-records) :instr]
                      [(:pc control-records) :pc]
                      [(:buses control-records) :buses]
                      [std-logic
                       :slp
                       :event-ack
                       :debug])
     :nillable-outputs
     #{:shiftfunc :imm-val :ma-wr
       :arith-func :arith-sr-func
       :logic-func :logic-sr-func
       :zbus-sel
       :mem-addr-sel :mem-size :mem-wdata-sel
       ;; need xbus-sel and ybus-sel to not be SEL_REG when not set so
       ;; to avoid false positive register conflicts causing stalls       
       ;;:xbus-sel :ybus-sel
       :regnum-w :regnum-x :regnum-y :regnum-z}
     :multi-stage-signals (find-multi-state-signals pipelines)}))
