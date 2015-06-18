(ns cpugen.vmagic
  (:require [clojure.string :as s])
  (:import
   clojure.lang.Keyword
   clojure.lang.Symbol
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
   de.upb.hni.vmagic.output.VhdlOutput
   [de.upb.hni.vmagic.util
    VhdlCollections
    Comments]
   [de.upb.hni.vmagic.statement
    AssertionStatement
    CaseStatement
    CaseStatement$Alternative
    ExitStatement
    ForStatement
    IfStatement
    LoopStatement
    NextStatement
    NullStatement
    ProcedureCall
    ReportStatement
    ReturnStatement
    SequentialStatement
    SequentialStatementVisitor
    SignalAssignment
    VariableAssignment
    WaitStatement
    WhileStatement]
   [de.upb.hni.vmagic.expression
    Abs
    Add
    AddingExpression
    Aggregate
    Aggregate$ElementAssociation
    And
    BinaryExpression
    Concatenate
    Divide
    Equals
    Expression
    Expressions
    ExpressionVisitor
    FunctionCall
    GreaterEquals
    GreaterThan
    LessEquals
    LessThan
    Literal
    LogicalExpression
    Minus
    Mod
    Multiply
    MultiplyingExpression
    Name
    Nand
    Nor
    Not
    NotEquals
    Or
    Parentheses
    Plus
    Pow
    Primary
    QualifiedExpression
    QualifiedExpressionAllocator
    RelationalExpression
    Rem
    Rol
    Ror
    ShiftExpression
    Sla
    Sll
    Sra
    Srl
    Subtract
    SubtypeIndicationAllocator
    TypeConversion
    UnaryExpression
    Xnor
    Xor]
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
    SubprogramBody
    FunctionDeclaration
    SignalDeclaration
    ConstantDeclaration
    VariableDeclaration
    Component
    Subtype
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
    Type
    ConstrainedArray
    IntegerType
    IndexSubtypeIndication
    RecordType
    RecordType$ElementDeclaration
    EnumerationType]
   [de.upb.hni.vmagic.object
    Constant
    Slice
    VhdlObject
    VhdlObject$Mode
    VhdlObjectProvider
    Signal
    ArrayElement
    RecordElement
    Variable
    SignalAssignmentTarget
    VariableAssignmentTarget
    AttributeExpression]
   [de.upb.hni.vmagic.output
    VhdlOutput]
   [de.upb.hni.vmagic.concurrent
    ProcessStatement
    ConditionalSignalAssignment
    ConditionalSignalAssignment$ConditionalWaveformElement
    SelectedSignalAssignment
    SelectedSignalAssignment$SelectedWaveform
    SelectedSignalAssignment
    AbstractComponentInstantiation
    ComponentInstantiation]))

(defn vstr [^VhdlElement e]
      (VhdlOutput/toVhdlString e))

(defn set-comments [entity & comments]
  (let [all-entities (flatten [entity])]
    (if (and (seq all-entities) (seq comments))
      (Comments/setComments (first all-entities) (into-array (map #(str " " %) comments)))))
  entity)

(defn vrange [a dir b]
  (Range. a (case dir :downto Range$Direction/DOWNTO :to Range$Direction/TO) b))

(defn range-to [a b] (vrange a :to b))
(defn range-downto [a b] (vrange a :downto b))

(def std-logic StdLogic1164/STD_LOGIC)
(defn std-logic-vector
  ([n] (StdLogic1164/STD_LOGIC_VECTOR n))
  ([l r] (StdLogic1164/STD_LOGIC_VECTOR (range-downto l r))))

(defn func-dec [name return-type & args]
  (FunctionDeclaration. name return-type
                        (into-array VhdlObjectProvider args)))

(defn func-call [fn & args]
  (let [call (FunctionCall. fn)
        params (.getParameters call)]
    (doseq [arg args]
      (.add params arg))
    call))

(defn assoc-elem
  ([n] (AssociationElement. n))
  ([n v] (AssociationElement. n v)))

(defn func-call-pos [fn & args]
  (apply func-call fn (map assoc-elem args)))

(defn clj-name [name]
  (s/lower-case
   (s/replace
    (if-let [[m _ n]
             (re-matches
              #"(id_|ex_|wb_|)(.*)" name)]
      n
      name)
    #"_" "-")))

(defn vhdl-name ^String [n]
  (s/replace (if (keyword? n) (name n) n)  #"-" "_"))

(def std-match
  (func-dec "std_match" Standard/BOOLEAN
            (Constant. "L" StdLogic1164/STD_LOGIC_VECTOR)
            (Constant. "R" StdLogic1164/STD_LOGIC_VECTOR)))

(defprotocol ListInside
  (declarations ^java.util.List [e])
  (statements ^java.util.List [e])
  (else-statements ^java.util.List [e])
  (elements ^java.util.List [e])
  (sensitivities ^java.util.List [e])
  (ports ^java.util.List [e])
  (port-map ^java.util.List [e])
  (generic-map ^java.util.List [e]))

(defprotocol ValueOf
  (zero-val ^Expression [e])
  (num-val ^Expression [e v]))

(defprotocol VHDLType
  (type-of [e]))

(defprotocol Copyable
  (copy [e]))

(defprotocol Assignable
  (assign [e v])
  (varassign [e v]))

(extend-type Architecture
  ListInside
  (declarations [e] (.getDeclarations e))
  (statements [e] (.getStatements e)))

(extend-type Entity
  ListInside
  (declarations [e] (.getDeclarations e))
  (statements [e] (.getStatements e))
  (ports [e] (.getPort e)))

(extend-type ProcessStatement
  ListInside
  (declarations [e] (.getDeclarations e))
  (statements [e] (.getStatements e))
  (sensitivities [e] (.getSensitivityList e)))

(extend-type PackageDeclaration
  ListInside
  (declarations [e] (.getDeclarations e)))

(extend-type PackageBody
  ListInside
  (declarations [e] (.getDeclarations e)))

(extend-type VhdlFile
  ListInside
  (elements [e] (.getElements e)))

(extend-type de.upb.hni.vmagic.statement.IfStatement
  ListInside
  (statements [e] (.getStatements e))
  (else-statements [e] (.getElseStatements e)))

(extend-type de.upb.hni.vmagic.statement.IfStatement$ElsifPart
  ListInside
  (statements [e] (.getStatements e)))

(extend-type SubprogramBody
  ListInside
  (declarations [e] (.getDeclarations e))
  (statements [e] (.getStatements e)))

(defn get-id [e] (.getIdentifier e))

(defn add-all [entity list-fn & args]
  (let [args (filter identity (flatten args))]
    (if (seq args)
      (.addAll ^java.util.List (list-fn entity) args)))
  entity)

(defn add-declarations [entity & args]
  (apply add-all entity declarations
           (->> (flatten args)
                (filter identity)
                (map (fn [d]
                       (cond
                        (instance? Constant d) (ConstantDeclaration. (into-array [d]))
                        (instance? Signal d) (SignalDeclaration. (into-array [d]))
                        (instance? Variable d) (VariableDeclaration. (into-array [d]))
                        :else d))))))

(defn assign-zero [s]
  (assign s (zero-val s)))

(defn varassign-zero [s]
  (varassign s (zero-val s)))

(defn- bad-zero [e]
  (throw (UnsupportedOperationException. (str "Cannot zero " e))))

(defn- bad-value [e v]
  (throw (UnsupportedOperationException. (str "Cannot assign" v "to" e))))

(extend-type Subtype
  ValueOf
  (zero-val [e]
    (cond
     (= std-logic e) StdLogic1164/STD_LOGIC_0
     :else (bad-zero e)))
  (num-val [e v]
    (cond
     (= std-logic e)
     (case v
       0 StdLogic1164/STD_LOGIC_0
       1 StdLogic1164/STD_LOGIC_1)
     :else (bad-value e v)))
  VHDLType
  (type-of [e]
    (cond
     (= std-logic e) std-logic
     :else (bad-zero e))))

(defn abs [x]
  (if (< x 0) (- x) x))

(defn- zero-pad [s width]
  (if (> (count s) width)
    (.substring s (- (count s) width))
    (str (apply str (repeat (max 0 (- width (count s))) "0")) s)))

(defn- num-literal [width val]
  (if (zero? (mod width 4))
    ;; output hex string
    (HexLiteral. (zero-pad (Long/toHexString val) (quot width 4)))
    (StringLiteral. (zero-pad (Long/toBinaryString val) width))))

(extend-type IndexSubtypeIndication
  ValueOf
  (zero-val [e]
    (cond
     (and #_(= StdLogic1164/STD_LOGIC (.getBaseType e))
          (= 1 (count (.getRanges e))))
     (let [r (first (.getRanges e))
           f (.getFrom r)
           t (.getTo r)]
       (if (and (instance? DecimalLiteral f)
                (instance? DecimalLiteral t))
         (let [n (inc (abs (- (Integer/parseInt (.getValue f))
                              (Integer/parseInt (.getValue t)))))]
           (num-literal n 0))
         (bad-zero e)))
     :else (bad-zero e)))
  (num-val [e v]
    (cond
     (and #_(= StdLogic1164/STD_LOGIC (.getBaseType e))
          (= 1 (count (.getRanges e))))
     (let [r (first (.getRanges e))
           f (.getFrom r)
           t (.getTo r)]
       (if (and (instance? DecimalLiteral f)
                (instance? DecimalLiteral t))
         (let [n (inc (abs (- (Integer/parseInt (.getValue f))
                              (Integer/parseInt (.getValue t)))))]
           (num-literal n v))
         (bad-value e v)))
     :else (bad-value e v))))

(extend-type RecordType
  ListInside
  (elements [e] (.getElements e))
  ValueOf
  (zero-val [e]
    (let [agg (Aggregate.)]
      (doseq [elem (.getElements e)
              ident (.getIdentifiers elem)]
        (.createAssociation agg (zero-val (.getType elem))))
      agg)))

(extend-type EnumerationType
  ValueOf
  (zero-val [e]
    (first (.getLiterals e))))

(extend-type AbstractComponentInstantiation
  ListInside
  (port-map [e] (.getPortMap e))
  (generic-map [e] (.getGenericMap e)))

(extend-type Signal
  ValueOf
  (zero-val [e]
    (zero-val (.getType e)))
  (num-val [e v]
    (num-val (.getType e) v))
  VHDLType
  (type-of [e]
    (.getType e))
  Copyable
  (copy [e]
    (Signal. (.getIdentifier e) (.getMode e) (.getType e) (.getDefaultValue e))))

(extend-type Variable
  ValueOf
  (zero-val [e]
    (zero-val (.getType e)))
  (num-val [e v]
    (num-val (.getType e) v))
  VHDLType
  (type-of [e]
    (.getType e))
  Copyable
  (copy [e]
    (Variable. (.getIdentifier e) (.getType e) (.getDefaultValue e))))

(extend-type ConstrainedArray
  ValueOf
  (zero-val [e]
    (num-val e 0)
    #_(if (and (= std-logic (.getElementType e))
             (= 1 (count (.getIndexRanges e))))
      (let [r (first (.getIndexRanges e))
            from (.getFrom r)
            to (.getTo r)]
        (if (and
             (instance? DecimalLiteral from)
             (instance? DecimalLiteral to))
          (num-literal (inc (Math/abs (- (Long/parseLong (.getValue from))
                                         (Long/parseLong (.getValue to)))))
                       0)
          (bad-zero e)))      
      (bad-zero e)))
  (num-val [e v]
    (if (and (= std-logic (.getElementType e))
             (= 1 (count (.getIndexRanges e))))
      (let [r (first (.getIndexRanges e))
            from (.getFrom r)
            to (.getTo r)]
        (if (and
             (instance? DecimalLiteral from)
             (instance? DecimalLiteral to))
          (num-literal (inc (Math/abs (- (Long/parseLong (.getValue from))
                                         (Long/parseLong (.getValue to)))))
                       v)
          (bad-value e v)))      
      (bad-value e v))))

(defn- assign-fn [s v]
  (SignalAssignment. s (if (number? v) (num-val s v) v)))
(defn- varassign-fn [s v]
  (VariableAssignment. s (if (number? v) (num-val s v) v)))

(extend Signal
  Assignable
  {:assign assign-fn})

(extend Variable
  Assignable
  {:varassign varassign-fn})

(doseq [cls [Aggregate ArrayElement Slice RecordElement]]
  (extend cls
    Assignable
    {:assign assign-fn
     :varassign varassign-fn}))

(defn record-element-type
  "Return the type of a RecordType element. The getType method in
  RecordType doesn't work."
  [^RecordElement elem]
  (let [name (.getElement elem)]
    (some (fn [^RecordType$ElementDeclaration e]
            (when (some #(= name %) (.getIdentifiers e))
              (.getType e)))
          (elements (.getType (.getPrefix elem))))))

(extend-type RecordElement
  ValueOf
  (zero-val [e]
    (zero-val (record-element-type e)))
  (num-val [e v]
    (num-val (record-element-type e) v))
  VHDLType
  (type-of [e]
    (record-element-type e)))

(extend-type IndexSubtypeIndication
  VHDLType
  (type-of [e]
    (.getBaseType e)))

#_(defn array-element-type
  "Return the type of an ArrayElement element. The getType method in
  ArrayElement doesn't work."
  [^ArrayElement elem]
  (println elem (.getPrefix elem) (type-of (type-of (.getPrefix elem))))
  (.getElementType (.getPrefix elem))
  #_(let [array (.getPrefix elem)
        array-type (.getType array)]
    (if (and (instance? IndexSubtypeIndication array-type)
             (= (count (.getRanges array-type))
                (count (.getIndices elem))))
      (.getBaseType array-type)
      (throw (IllegalStateException. (str "Unable to determine type of array" elem array))))))

#_(extend-type ArrayElement
  ValueOf
  (zero-val [e]
    (zero-val (array-element-type e)))
  (num-val [e v]
    (num-val (array-element-type e) v))
  VHDLType
  (type-of [e]
    (array-element-type e)))

(extend-type LoopStatement
  ListInside
  (statements [e] (.getStatements e)))

(extend-type CaseStatement$Alternative
  ListInside
  (statements [e] (.getStatements e)))

(defn if-stmt [& cond-body]
  (when (< (count cond-body) 2)
    (throw (Exception. "if-stmt must take more than 1 argument")))
  (let [[c1 b1 & cond-body] cond-body
        stmt (add-all (IfStatement. c1) statements b1)]
    (doseq [[c b] (partition 2 cond-body)]
      (add-all (.createElsifPart stmt c) statements b))
    (when (not (even? (count cond-body)))
      (add-all stmt else-statements (last cond-body)))
    stmt))

(defn cond-assign [sig & val-conds]
  (ConditionalSignalAssignment.
   sig
   (->> val-conds
        (map #(if (number? %) (num-val sig %) %))
        (partition 2 2 [nil])
        (map (fn [[val cond]]
               (ConditionalSignalAssignment$ConditionalWaveformElement.
                [(WaveformElement. (if (number? val) (num-val sig val) val))]
                cond))))))

(defn select-assign [target expr & val-cmpvals]
  (let [stmt (SelectedSignalAssignment.
              expr target)]
    (.addAll (.getSelectedWaveforms stmt)
      (->> val-cmpvals
           ;; convert numbers to proper values
           (partition 2 2 [Choices/OTHERS])
           (map (fn [[a b]] [(if (number? a) (num-val target a) a)
                            (map #(if (number? %) (num-val expr %) %) (if (vector? b) b [b]))]))
           (map (fn [[a bs]]
                  (SelectedSignalAssignment$SelectedWaveform. a (into-array bs))))))
    stmt))

(defn rec-elem [record & names]
  (case (count names)
    0 (throw (IllegalArgumentException. "rec elem needs at least one name"))
    1 (RecordElement. record (first names))
    (apply rec-elem (RecordElement. record (first names)) (rest names))))

(defn binary-exp [f]
  (fn [a & bs]
    (if-let [r (reduce
                (fn [a b]
                  (cond
                   (and a b) (f a b)
                   a a
                   b b))
                a bs)]
      r
      (throw (NullPointerException. "All arguments nil")))))

(defn paren [x]
  (Parentheses. x))

(defn binary-exp-paren [f]
  (let [fr (binary-exp f)]
    (fn [a & args]
      (let [r (apply fr a args)]
        (if (= a r)
          r
          (paren r))))))

(def v-or (binary-exp-paren #(Or. %1 %2)))
(def v-and (binary-exp-paren #(And. %1 %2)))
(def v-xor (binary-exp-paren #(Xor. %1 %2)))
(def v-nor (binary-exp-paren #(Nor. %1 %2)))
(def v-nand (binary-exp-paren #(Nand. %1 %2)))
(def v-xnor (binary-exp-paren #(Xnor. %1 %2)))
(def v-+ (binary-exp-paren #(Add. %1 %2)))

(def v-cat (binary-exp #(Concatenate. %1 %2)))
(defn v-not [exp] (Not. exp))

(defn- unify-nums [a b]
  [(if (number? a) (num-val b a) a)
   (if (number? b) (num-val a b) b)])

(defn v= [a b]
  (let [[a b] (unify-nums a b)]
    (Equals. a b)))
(defn vnot= [a b]
  (let [[a b] (unify-nums a b)]
    (NotEquals. a b)))

(defn attr-event [exp]
  (AttributeExpression. exp (Attribute. "event" Standard/STRING)))

(defn attr-pos [exp param]
  (AttributeExpression. exp (Attribute. "pos" Standard/STRING) param))

(defn attr-val [exp param]
  (AttributeExpression. exp (Attribute. "val" Standard/STRING) param))

(defn- set-mode [mode sigs]
  (doseq [^Signal s (flatten sigs)]
    (.setMode s mode))
  sigs)

(defn add-in-ports [entity args]
  (add-all entity ports
           (set-mode VhdlObject$Mode/IN (map copy args))))

(defn add-out-ports [entity args]
  (add-all entity ports
           (set-mode VhdlObject$Mode/OUT (map copy args))))

(defn add-inout-ports [entity args]
  (add-all entity ports
           (set-mode VhdlObject$Mode/INOUT (map copy args))))

(defn signal [name type]
  (Signal. name type))

(defn gen-const
  ([name record]
     (gen-const name record {}))
  ([name record vals]
     (Constant. name record
                (let [agg (Aggregate.)]
                  (doseq [elem (.getElements record)
                          ident (.getIdentifiers elem)]
                    (.createAssociation
                     agg (or (get vals ident)
                             (zero-val (.getType elem)))
                     (into-array [(Signal. ident (.getType elem))]))) agg))))

(defn choices [& r]
  (into-array Choice r))

(defn ranges [& r]
  (into-array DiscreteRange r))

(defprotocol ToSignal
  (create-signal ^Signal [v type]))

(extend-type Keyword
  ToSignal
  (create-signal [k type]
    [k (Signal. (vhdl-name k) type)]))

(extend-type Symbol
  ToSignal
  (create-signal [s type]
    (let [key (or (:key (meta s)) (keyword (name s)))]
      [key (Signal. (vhdl-name (name s)) type)])))

(defrecord KeySig [key name]
  ToSignal
  (create-signal ^Signal [r type]
    [key (Signal. name type)]))

(defn create-signals [& descs]
  (into {} (for [[type & sigs] descs
                 s (flatten sigs)]
             (create-signal s type))))

(defn record-type [name & elements]
  (let [record (RecordType. name)
        sigs (for [[type & keys] elements
                   k (flatten keys)]
               (create-signal k type))]
    (doseq [[_ sig] sigs]
      (.createElement record (.getType sig) (into-array [(get-id sig)])))
    record))

(defn slice-to [exp a b]
  (Slice. exp (range-to a b)))

(defn slice-downto [exp a b]
  (Slice. exp (range-downto a b)))

(defn v-equals [a b]
  (cond
   (and
    (instance? StringLiteral a)
    (instance? StringLiteral b))
   (= (.getString a) (.getString b))
   (and
    (instance? HexLiteral a)
    (instance? HexLiteral b))
   (= (str a) (str b))
   :else (= a b)))

(defn qual-exp [type op]
  (QualifiedExpression. type op))

(defn pos-agg [& exps]
  (let [agg (Aggregate.)]
    (doseq [exp exps]
      (.createAssociation agg exp))
    agg))

(defn named-agg [& exp-choice-pairs]
  (let [agg (Aggregate.)]
    (if (some nil? exp-choice-pairs)
      (throw (IllegalArgumentException. "no arg can be nil")))
    (if (zero? (count exp-choice-pairs))
      (throw (IllegalArgumentException. "aggregate cannot contin zero arg associations")))

    (doseq [[exp choice] (partition 2 2 [nil] exp-choice-pairs)]
      (.createAssociation agg
                          exp
                          (cond
                           (nil? choice) [Choices/OTHERS]
                           ;; Create a fake signal just to pass the
                           ;; name. Is there a better way?
                           (string? choice) [(Signal. choice std-logic)]
                           (vector? choice) (into-array choice)
                           :else (into-array [choice]))))
    agg))

(defn instantiate-component
  ([name comp-or-entity ports]
     (instantiate-component name comp-or-entity ports nil))
  ([name comp-or-entity ports generics]
     (-> (ComponentInstantiation. name
                                  (if (instance? Entity comp-or-entity)
                                                (Component. comp-or-entity)
                                                comp-or-entity))
         (add-all
          port-map (map #(apply assoc-elem %) ports))
         (add-all
          generic-map (map #(apply assoc-elem %) generics)))))

(defn case-statement [input & cases]
  (when-not (zero? (mod (count cases) 2))
    (throw (IllegalArgumentException. "requires even number of cases")))
  (let [cs (CaseStatement. input)]
    (doseq [[c stmts] (partition 2 cases)]
      (let [alt (.createAlternative cs (choices c))]
        (add-all alt statements stmts)))
    cs))

(defn return-stmt [x]
  (ReturnStatement. x))
