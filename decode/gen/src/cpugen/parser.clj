(ns cpugen.parser
  (:require [clojure.data.csv :as csv]
            [clojure.java.io :as io]
            [clojure.string :as s]
            [cpugen [logic :as logic]])
  (:use [clojure.core.match :only (match)])
  (:import org.odftoolkit.simple.Document))

(def ^:private hdr-keys
  {"TABLE" :table
   "Format" :format
   "State" :state
   "Instruction" :name
   "Plane" :plane
   "Op Code" :op
   "Operation" :desc
   "XBUS" :x
   "YBUS" :y
   "FUNC" :func
   "ALU X" :alux
   "ALU Y" :aluy
   "ZBUS" :z
   "WBUS" :w
   "PC" :inc-pc
   "IF ADDY" :if-addr
   "Latch S_MAC" :latch-s-mac
   "PR" :pr
   "IF ISSUE" :if-issue
   "DISPATCH" :dispatch
   "DEBUG" :debug
   "MAC STAGE" :mac-stage
   "MAC BUSY" :mac-busy
   "MAC OP" :mac-op
   "MAC STALL SENSE" :mac-stall-sense
   "MACIN_1" :macin1
   "MACIN_2" :macin2
   "MACH" :mach
   "MACL" :macl
   "EVENT" :event
   "HALT" :halt
   "ILEVEL CAPTURE" :ilevel-capture
   "DELAY JMP" :delay-jump
   "MA OP" :ma-op
   "MA MASK" :ma-mask
   "MA SIZE" :ma-size
   "MA DATA" :ma-data
   "MA ADDY" :ma-addr
   "MA LOCK" :ma-lock
   "MASK INT" :mask-int

   "ZBUS SEL" :zbus
   "SR" :sr
   "ARITH" :arith
   "ARITH SR" :arith-sr
   "LOGIC" :logic
   "LOGIC SR" :logic-sr
   "CARRYIN EN" :carryin-en
   "SHIFT" :shift
   "MANIP" :manip})

(defn- reverse-map [m]
  (into {} (map (fn [[a b]] [b a]) m)))

(def ^:private inv-hdr-keys (reverse-map hdr-keys))

(def ^:private hdr-keys (into {} (map (fn [[k v]]
                              [(s/lower-case k) v]) hdr-keys)))

(defn- row-cell [row i]
  (.getStringValue (.getCellByIndex row i)))

(defn- load-ods
  "Returns vector of row vectors with cell contents as strings"
  [file]
  (with-open [doc (Document/loadDocument file)]
    (let [table (first (.getTableList doc))
          ;; scan the cells to see how far the data goes. .getRowCount
          ;; and .getColumnCount seem to hang and return huge numbers
          num-cols
          (count (take-while (comp not s/blank?)
                             (map (partial row-cell (.getRowByIndex table 0)) (range))))
          rows (map (fn [row] (mapv #(row-cell row %) (range num-cols)))
                    (iterator-seq (.getRowIterator table)))]
      (->> rows
           ;; decide rows end when see 20 blank rows
           (partition 20 1)
           (take-while
            (fn [rows] (some #(not (s/blank? %)) (apply concat rows))))
           (mapv first)))))

(defn- load-file-lines [file]
  ;; determine how to load file based on extension
  (if-let [[_ ext] (re-find #"\.([^.]*)$" (.getName (io/file file)))]
    (case ext
      "csv" (with-open [in-file (io/reader file)] (doall (csv/read-csv in-file)))
      "ods" (load-ods file)
      (throw (Exception. (str "Unrecognized file extension" \" ext \"))))
    (throw (Exception. (str "No file extension found in " \" file \")))))

(defn- read-file [file]
  (let [lines (load-file-lines file)
        hdr (first lines)
        hdr (map #(let [x (s/lower-case (s/trim %))]
                    (get hdr-keys x x)) hdr)]
    (->>
     (map-indexed
      (fn [n line]
        (into {:line (+ n 2)}
              (->> line
                   ;; trim all values
                   (map #(s/trim %))
                   ;; zip with hdr keys
                   
                   (zipmap hdr)
                   ;; filter out empty or "-" values
                   (filter (fn [[k v]]
                             (not (or (.isEmpty v) (= "-" v))))))))
      (rest lines))

     ;; remove empty lines
     (filter #(seq (dissoc % :line)))
     doall)))


(defn- remove-wr-prefix [^String v]
  (if (.startsWith v "wr ")
    (.substring v 3)
    v))

(defn- clean-chars [s]
  (reduce
    (fn [s [r v]]
      (s/replace s r v))
    s
    [[#"≥" ">="]
     [#"→" "->"]
     [#"←" "<-"]
     [#"×" "x"]
     [#"–" "-"]]))

(def ^:private yes-map
  {"yes" true "y" true})

(defn- parse-const [v]
  (if-let [[sign mult]
           (seq (or
                 (rest (re-matches
                        #"([us])(?:\s*[x*]\s*([0-9]+))?" v))
                 (reverse (rest (re-matches
                                 #"[x*]\s*([0-9]+)\s*([us])" v)))))]
    [(case sign "u" :u "s" :s) (or (when mult (Long/parseLong mult)) 1)]
    (when-let [[all sign base num] (re-matches #"([+-])?(0[xb])?([0-9]+)" v)]
      (Long/parseLong (str sign num) (get {"0x" 16
                                           "0b" 2} base 10)))))

(def immediate-cols [:x :y :aluy])

(def register-map
  {"r0" [:r 0]
   "rn" :rn
   "rm" :rm
   "r15" [:r 15]
   "gbr" (with-meta [:r 16] {:name "GBR"})
   "vbr" (with-meta [:r 17] {:name "VBR"})
   "pr" (with-meta [:r 18] {:name "PR"})
   "temp0" (with-meta [:r 19] {:name "TEMP0"})
   "temp1" (with-meta [:r 20] {:name "TEMP1"})})

(def ^:private sanitize-fns
  {:line #(Integer/parseInt %)
   :table (fn [v]
            (when-let [[m name num]
                       (re-matches
                        #"([A-Z])\.([0-9]+)" v)]
              [(keyword (s/lower-case name)) (Integer/parseInt num)]))
   :name clean-chars
   :desc clean-chars
   :plane #(get {"system" 1} % 0)
   :format {"0" :0
            "n" :n
            "m" :m
            "nm" :nm
            ;; Why is "mn" in the spreadsheet??
            "mn" :nm
            "md" :md
            "nd4" :nd4
            "nmd" :nmd
            "d8" :d8
            "d12" :d12
            "nd8" :nd8
            "i8" :i8
            "ni" :ni}
   :x (fn [v] (or (parse-const v)
                 ((merge register-map
                         {"pc" :pc
                          "const" :const
                          "w" :w}) v)))
   :y (fn [v] (or (parse-const v)
                 ((merge register-map
                         {"pc" :pc
                          "mach" :mach
                          "macl" :macl
                          "const" :const
                          "sr" :sr}) v)))
   :alux {"fc" :fc
          "rotcl" :rotcl
          "zero" :zero}
   :aluy (fn [v] (or (parse-const v)
                    ({"r0" :r0} v)))
   :w (comp
       (merge
        register-map
        {"sr" :sr})
       remove-wr-prefix)
   :z (comp
       (merge
        register-map
        {"pc" :pc
         "t(pc)" :pc-t
         "nt(pc)" :pc-nt
         "sr" :sr})
       remove-wr-prefix)
   :pr {"rd pc" :pc}
   :if-addr {"zbus" :z}
   :ma-op {"read" :read
           "write" :write}
   :ma-mask {"t" :t "nt" :nt}
   :ma-issue {"set" true}
   :ma-size #(Integer/parseInt %)
   :ma-data {"zbus" :z
             "ybus" :y}
   :ma-addr {"zbus" :z
             "xbus" :x
             "ybus" :y}
   :ma-lock yes-map
   :inc-pc {"inc" true
            "hold" false}
   :delay-jump {"set" true}
   :if-issue (merge yes-map {"t" :t "nt" :nt})
   :dispatch (merge yes-map {"t" :t "nt" :nt})
   :debug yes-map
   :mac-stage {"ex" :ex "wb" :wb}
   :mac-busy {"busy" :busy
              "not stall" :not-stall}
   :mac-op {"macl" :macl
            "macw" :macw
            "dmulsl" :dmulsl
            "dmulul" :dmulul
            "mull" :mull
            "mulsw" :mulsw
            "muluw" :muluw}
   :mac-stall-sense yes-map
   :macin1 {"xbus" :x
            "wbus" :w
            "zbus" :z}
   :macin2 {"ybus" :y
            "wbus" :w
            "zbus" :z}
   :mach {"clear" :clear
          "load" :load}
   :macl {"clear" :clear
          "load" :load}
   :halt {"set" true}
   :event {"ack" :ack}
   :ilevel-capture yes-map
   :mask-int yes-map

   :zbus {"arith" :arith
          "logic" :logic
          "shift" :shift
          "manip" :manip
          "y" :ybus
          "w" :wbus}
   :sr {"arith" :arith
         "logic" :logic
         "z" :zbus
         "w" :wbus
         "div0u" :div0u
         "int_mask" :int-mask
         "t=0" [:t :clear]
         "t=1" [:t :set]
         "shift" [:t :shift]
         "carry" [:t :carry]}
   :arith {"add" :add "sub" :sub}
   :arith-sr {"ugrter_eq" :u>=
              ">=" :u>=
              "sgrter_eq" :s>=
              "s>=" :s>=
              "ugrter" :u>
              ">" :u>
              "sgrter" :s>
              "s>" :s>
              "zero" :zero
              "overunderflow" :overunderflow
              "div0s" :div0s
              "div1" :div1}
   :logic {"and" :and
           "or" :or
           "not" :not
           "xor" :xor}
   :logic-sr {"byte_eq" :b=
              "byte =" :b=
              "byte=" :b=
              "zero" :zero}
   :carryin-en {"1" 1}
   :latch-s-mac {"1" 1}
   :shift {"rotate" :rotate
           "rotatec" :rotatec
           "shiftl" :logic
           "shifta" :arith}
   :manip
   (fn [v]
     (get
      {"xtract" :xtract
       "set b7" :bit7} v
       (when-let [manip-matches (re-matches #"(ext|swap) *([a-z]+[0-9]*)?" v)]
         (let [[m name opt] manip-matches]
           (if opt
             [(keyword name) (keyword opt)]
             (keyword name))))))})

(def ^:dynamic *errors* nil)

(defn- log-error [line name msg & vals]
  (when-let [errs *errors*]
    (swap! errs conj {:line line :name name :msg msg :vals vals})))

(defn- log-bad-value [line name k v]
  (log-error line name
             "Invalid value"
             (str (get inv-hdr-keys k k) "=\"" v "\"")))

(defn- sanitize-common
  [common line]
  (into {}
        (->> common
             (map (fn [[k v]]
                    (let [v2 ((get sanitize-fns k identity) v)]
                      (if (nil? v2)
                        (do
                          (log-bad-value line (:name common) k v)
                          [k v])
                        [k v2])))))))

(defn- sanitize-slot
  "given a map of the key/values for a single microcode op (excluding
  the fields common to all microcode ops in an opcode), returns a
  santized version of the map. Values are cleaned up and converted to
  more useful representations. Related fields are grouped together
  into sub-maps"
  [slot name]
  (let [r
        (into
         {}
         (->> slot
              (map (fn [[k v]]
                     (let [v2 ((get sanitize-fns k identity)
                               (s/lower-case v))]
                       (if (nil? v2)
                         (do
                           (log-bad-value (:line slot) name k v)
                           [k v])
                         [k v2]))))))
        ma-keys-map {:ma-addr :addr
                     :ma-data :data
                     :ma-size :size
                     :ma-op :op
                     :ma-mask :mask}
        mac-keys [:mac-stage :mac-op :mac-busy :macin1 :macin2 :mach :macl]
        
        ;; group memory access
        r (apply dissoc
                 (if (some identity ((apply juxt (keys ma-keys-map)) r))
                   (assoc r :ma
                          (into {}
                                (map (fn [[k v]] [(ma-keys-map k) v])
                                     (select-keys r (keys ma-keys-map)))))
                   r)
                 (keys ma-keys-map))]

    ;; group mult related
    (apply dissoc
     (if (:mac-stage r)
       (assoc r :mac
              (into {}
                    (map (fn [[k v]]
                           [({:mac-stage :stage
                              :mac-busy :busy
                              :mac-op :op
                              :macin1 :in1
                              :macin2 :in2
                              :mach :h
                              :macl :l} k) v])
                         (select-keys r mac-keys))))
       r)
     mac-keys)))

(defn log2 [n]
  (- Long/SIZE (Long/numberOfLeadingZeros n)))

(defn- extract-imm
  "convert references to immediate values like [:s 1] to the width of
  immediate value in the opcode. Also convert multiplications to shift
  amounts" [common slot]

  ;; determine width of immediate value from opcode format
  (let [bit-width (get {:md 4
                        :nd4 4
                        :nmd 4
                        :d8 8
                        :d12 12
                        :nd8 8
                        :i8 8
                        :ni 8} (:format common) 0)]
    (merge slot
           (into {}
                 (map (fn [[k v]]
                        (if (match [v]
                                   [[(:or :u :s) (m :guard number?)]] true)
                          (let [[s m] v]
                            (if (zero? bit-width)
                              (println "FAILED imm" v (:format common) (:name common) (:line slot)))
                            [k
                             (-> [s bit-width (if (= m 1) 0
                                                  (if (or (< m 1) (not (zero? (mod m 2))))
                                                    0
                                                    ;; log_2 of long
                                                    (- (dec Long/SIZE)
                                                       (Long/numberOfLeadingZeros m))))]
                                 (with-meta 
                                   {:orig v}))])))
                      (select-keys slot immediate-cols))))))

(defn load-spreadsheet [file]
  (let [ops
        (->> (read-file file)
             (partition-by #(select-keys % [:name]))
             ;; store multiple slots for each opcode in :slots child vector
             (map (fn [op]
                    (let [common-keys [:name :plane
                                       :op :desc
                                       :table :format :state]
                          num-slots (count op)
                          common (select-keys (first op) common-keys)
                          ;; merge in default common values
                          common (merge {:plane "instr"} common)
                          common (sanitize-common
                                  common
                                  (:line (first op)))
                          name (:name common)]
                      (assoc
                        common
                        :slots
                        (->> op
                             ;; remove common keys and sanitize remaining values
                             (map
                              #(sanitize-slot (apply dissoc % common-keys) name))
                             ;; apply issue-dispatch default rules to
                             ;; slots. any final slot without issue or
                             ;; dispatch will have issue and dispatch
                             ;; set to true
                             (map
                              #(if (and (nil? (:if-issue %2)) (nil? (:dispatch %2)))
                                 (merge %1 %2)
                                 %2)
                              (concat (repeat (dec num-slots) nil)
                                      [{:if-issue true
                                        :dispatch true}]))
                             vec))))))
        ;; For opcodes with multiple cases (T=0, T=1), only the first
        ;; has :table, :format, :state. Duplicate those values to the
        ;; opcodes that follow, if they don't already have their own..
        ops (first (reduce
                    (fn [[ops prev] op]
                      (if (:format op)
                        [(conj ops op) (select-keys op [:table :format :state :desc])]
                        [(conj ops (merge op prev)) prev]
                        ))
                    [[] {}]
                    ops))
        ;; Change U*4 style immediates to refer to bits in the opcode
        ;; and left-shifts
        ops (mapv (fn [op]
                   (assoc op :slots
                          (mapv (fn [slot]
                                  (extract-imm op slot)) (:slots op))))
                  ops)]
    ops))


(defn sanity-check [ops]
  (doseq [op ops
          slot (:slots op)]
    (let [err (fn [msg & vals]
                (apply log-error (:line slot) (:name op) msg vals))]

      ;; if Rn or Rm are referenced, ensure opcode format contains them
      (let [fmt (:format op)]
        (if (some #{:rn} (vals slot))
          (when-not (#{:n :nm :nd4 :nmd :nd8 :ni} fmt)
            (err (str "Opcode format \"" (name fmt) "\" cannot use Rn"))))
        (if (some #{:rm} (vals slot))
          (when-not (#{:m :nm :md :nmd} fmt)
            (err (str "Opcode format \"" (name fmt) "\" cannot use Rm")))))

      (when-let [ma (:ma slot)]
        (let [ma (dissoc ma :mask)]
          (match [ma]
                 [({:op :read}
                   :only [:op :addr :size])] true
                 [({:op :write}
                   :only [:op :addr :size :data])] true
                 :else (err "Bad mem access" ma))))

      (let [m (:m slot) q (:q slot)]
        (if (and (or (= :clear m) (= :clear q))
                 (not= m q :clear))
          (err "m and q must both be cleared")))

      (let [is-const (fn [x] (if (number? x) x
                                (match [x]
                                       [[(:or :u :s) (n :guard number?) (m :guard number?)]] x)))
            imms (->> ((juxt :aluy :x :y) slot)
                     (map is-const)
                     (filter identity)
                     distinct
                     seq)]
        (when imms
          (when (> (count imms) 1)
            (err "more than one distinct immediate value used"))
          (doseq [imm imms]
            (when-let [[num-bits mult-factor]
                       (match [imm]
                              [[(:or :u :s) (n :guard number?) (m :guard number?)]] [n m])]))))

      (when-let [zbus (#{:arith :logic :shift :manip} (:zbus slot))]
        (if-not (get slot zbus)
          (err (str "Must set " (name zbus) " operation to set zbus"))))

      (when-let [sr ({:arith :arith-sr :logic :logic-sr :carry :arith} (:sr slot))]
        (if-not (get slot sr)
          (err (str "Must set " (name sr) " SR operation to set SR"))))

      (when (and (:carryin-en slot) (nil? (:arith slot)))
        (err (str "Can only use carry in when arith op specified")))
      (when (:arith-sr slot)
        (when (nil? (:arith slot))
          (err (str "Cannot set ARITH SR when ARITH func not set")))
        (when (not= (:sr slot) :arith)
          (err (str "ARITH SR func set but not used by SR"))))
      (when (:logic-sr slot)
        (when (nil? (:logic slot))
          (err (str "Cannot set LOGIC SR when LOGIC func not set")))
        (when (not= (:sr slot) :logic)
          (err (str "LOGIC SR func set but not used by SR"))))
      (when (and (:arith slot) (not= (:zbus slot) :arith) (not= (:sr slot) :arith))
        (err (str "Arith function set but arith result not used")))
      (when (and (:logic slot) (not= (:zbus slot) :logic) (not= (:sr slot) :logic))
        (err (str "Logic function set but logic result not used")))))

  (let [logic-ops
        (apply hash-set
               (map
                (fn [op]
                  {:name (:name op)
                   :op (:op op)
                   :line (:line (first (:slots op)))
                   :logic (apply hash-set (assoc (logic/str-to-logic-map :i (:op op))
                                            :p (:plane op)))})
                ops))]
    (loop [ops logic-ops]
      (when (seq ops)
        (let [op (first ops)
              matches (filter #(logic/intersects-sets? (:logic op) (:logic %)) (rest ops))]
          (when (seq matches)
            (log-error (:line op) (:name op)
                       (str "instruction overlaps others on lines "
                            (s/join ", " (sort (map :line matches)))))))
        (recur (rest ops))
        ))))

(defn all-slots [ops]
    (->> ops
       (mapcat :slots)))

(defn extract-slot-key [ops & ks]
  (->> ops
       (mapcat :slots)
       (map #(vals (merge (into {} (map vector ks (repeat nil)))
                          (select-keys % ks))))
       (filter #(some identity %))
       distinct))

(defn check-load
  "Load a CSV, collecting and printing any errors"
  [file]
  (binding [*errors* (atom [])]
    (let [ops (load-spreadsheet file)]
      (sanity-check ops)
      (let [errs (sort-by :line @*errors*)]
        (println (count errs) "errors loading" file)
        (doseq [{:keys [line name msg vals]} errs]
          (apply println (str line ":")
                 (str name ":") msg vals))
        (when (empty? errs)
          ops)))))
