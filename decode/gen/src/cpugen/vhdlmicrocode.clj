(ns cpugen.vhdlmicrocode
  (:require [clojure.string :as s]
            [cpugen [parser :as parser]])
  (:use [clojure.core.match :only (match)]))

(defn- so "set output signals to 1" [& sigs] (vec (map vector sigs (repeat 1))))

(defn- ao "assign output signals" [& sig-vals]
  (when (not (zero? (mod (count sig-vals) 2)))
    (throw (IllegalArgumentException. (str "ao only accepts an even number of arguments"))))
  (vec (map vec (partition 2 sig-vals))))

(defn reg-name [reg]
  (cond
   (= reg :rn) "Rn"
   (= reg :rm) "Rm"
   (number? reg) reg
   (keyword? reg) (s/upper-case (name reg))
   (:name (meta reg)) (:name (meta reg))
   :else (match [reg]
                [[:r n]] (str "R" n (meta reg))
                [[:s _ 0]] (str "CONST")
                [[:s _ n]] (str "CONST * " (long (Math/pow 2 n)))
                [[:u _ 0]] (str "UCONST")
                [[:u _ n]] (str "UCONST * " (long (Math/pow 2 n)))
                :else "?")))

(defn is-const [x]
  (if (number? x) x
      (match [x]
             [[(:or :u :s) (w :guard number?) (s :guard number?)]] x)))

(defn x-bus [x rn rm]
  (when-let [[val & assigns]
             (match [x]
                    [[:r n]] [:reg [:regnum-x n]]
                    [:rn] [:reg [:regnum-x rn]]
                    [:rm] [:reg [:regnum-x rm]]
                    [:pc] [x]
                    [(:or (a :guard number?) [(:or :u :s) _ _])] [:imm]
                    [:w] [:wbus])]
    (concat
     [(str "X = " (reg-name x))
      (when val
        [:xbus-sel val])]
     assigns)))

(defn y-bus [y rn rm]
  (when-let [[val & assigns]
             (match [y]
                    [[:r n]] [:reg [:regnum-y n]]
                    [:rn] [:reg [:regnum-y rn]]
                    [:rm] [:reg [:regnum-y rm]]
                    [(:or :pc :mach :macl :sr)] [y]
                    [(:or (a :guard number?) [(:or :u :s) _ _])] [:imm])]
    (concat
     [(str "Y = " (reg-name y))
      [:ybus-sel val]]
     assigns)))

(defn z-bus [z rn rm]
  (when-let [[sig val num cmt]
             (or
              (match [z]
                     [[:r n]] [:wrreg-z 1 n]
                     [:rn] [:wrreg-z 1 rn]
                     [:rm] [:wrreg-z 1 rm])
              (case z
                :pc [:wrpc-z]
                :pc-t [:wrpc-z :t nil "if (T) PC = Z"]
                :pc-nt [:wrpc-z :nt nil "if (not T) PC = Z"]
                :sr [:wrsr-z]))]
    [(if cmt cmt (str (reg-name z) " = Z"))
     [sig (if (nil? val) 1 val)]
     (when num [:regnum-z num])]))

(defn w-bus [w rn rm]
  (when-let [[sig num & assigns]
             (match [w]
                    [[:r n]] [:wrreg-w n]
                    [:rn] [:wrreg-w rn]
                    [:rm] [:wrreg-w rm]
                    [:pc] [:wrpc-z nil])]
    (concat
     [(str (reg-name w) " = W")
      [sig 1]
      (when num [:regnum-w num])]
     assigns)))

(defn- gen-zbus-comments [zbus arith logic
                          shift manip
                          carryin-en alux aluy]
  (let [operand (fn [x name]
                  (if (or (nil? x)
                          (and (instance? String x) (s/blank? x)))
                    name x))
        x (get {:xbus "X" :fc "(X & FC)" :rotcl "(2*X + T)" :zero "0"} alux
               (operand alux "X"))
        y (get {:ybus "Y" :imm "IMM" :r0 "R0"} aluy
               (operand aluy "Y"))
        r
        (case zbus
          :arith
          (s/join " " [x (case arith :add "+" :sub "-") y])
          :logic (s/join " " [x (name logic) y])
          :shift (s/join " " [x "shift" (name shift) y])
          :manip
          (match [manip]
                 [[:ext :sb]] (str "(int8) " y)
                 [[:ext :sw]] (str "(int16) " y)
                 [[:ext :ub]] (str "(uint8) " y)
                 [[:ext :uw]] (str "(uint16) " y)
                 :else (s/join " " [x manip y]))
          nil)]
    (when r
      [(str "Z = " r)])))

(defn- gen-mac [mac]
  (let [stage (:stage mac)
        ;; prefix register keywords with stage name
        [macsel1 macsel2 mulcom1 mulcom2 wrmacl wrmach]
        (map #(keyword (str (name stage) "-" (name %)))
             [:macsel1 :macsel2 :mulcom1 :mulcom2 :wrmacl :wrmach])]
    (concat
     (when-let [macin1 (:in1 mac)]
       (concat
        (ao macsel1 (case macin1
                      :x :xbus
                      :z :zbus
                      :w :wbus))
        (when-not (#{:clear :load} (:h mac))
          (ao mulcom1 1))))
     (when-let [macin2 (:in2 mac)]
       (concat
        (ao macsel2 (case macin2
                      :y :ybus
                      :z :zbus
                      :w :wbus))
        (when-let [op (:op mac)]
          (ao mulcom2 op))))
     ;; although spreadsheet differentiates between clear and
     ;; load, both are writing to the mac from data busses.
     (when (#{:clear :load} (:l mac))
       (so wrmacl))
     (when (#{:clear :load} (:h mac))
       (so wrmach))
     (when-let [busy (:busy mac)]
       (ao :mac-busy [stage busy])))))

(defn gen-ma [ma]
  (concat
   [(str (let [addr (str "MEM["
                         (case (:addr ma)
                           :x "X"
                           :z "Z"
                           :y "Y")
                         "]")]
           (if (= :write (:op ma))
             (str  addr " = " (case (:data ma) :y "Y" :z "Z"))
             (str "W = " addr)))
         " " ({8 "byte" 16 "word" 32 "long"} (:size ma)))]
   (ao :ma-issue (or (:mask ma) true))
   (ao :ma-wr (if (= :write (:op ma)) 1 0))
   (case (:addr ma)
     :x (ao :mem-addr-sel :xbus)
     :z (ao :mem-addr-sel :zbus)
     :y (ao :mem-addr-sel :ybus))
   (case (:data ma)
     :y (ao :mem-wdata-sel :ybus)
     :z (ao :mem-wdata-sel :zbus)
     nil nil)
   (ao :mem-size (case (:size ma) 8 :byte 16 :word 32 :long))))

(defn gen-assigns [op mc]
  (let [[rn rm]
        (case (:format op)
          :n [:ra nil]
          :m [nil :ra]
          :nm [:ra :rb]
          :md [nil :rb]
          :nd4 [:rb nil]
          :nmd [:ra :rb]
          :nd8 [:ra nil]
          :ni [:ra nil]
          nil)
        aluy (match [(:aluy mc)]
                    [:r0] :r0
                    [(:or (a :guard number?) [(:or :u :s) _ _])] :imm)]
    (concat
     (when-let [x (:x mc)]
       (x-bus x rn rm))
     (when-let [y (:y mc)]
       (y-bus y rn rm))

     (apply gen-zbus-comments
            ((juxt :zbus :arith :logic
                   :shift :manip :carryin-en :alux :aluy) mc))
     (when-let [x (:alux mc)] (ao :aluinx-sel x))
     (when aluy (ao :aluiny-sel aluy))
     (mapcat
      (fn [[k v]]
        (when-let [val (get mc k)]
          (ao v val)))
      {:arith :arith-func
       :arith-sr :arith-sr-func
       :logic :logic-func
       :logic-sr :logic-sr-func
       :manip :alumanip
       :shift :shiftfunc
       :zbus :zbus-sel})
     (when-let [ci-en (:carryin-en mc)]
       (ao :arith-ci-en ci-en))

     (match [(:sr mc)]
            [[:t t]] (ao :sr-sel :set-t
                         :t-sel t)
            [:wbus] (ao :wrsr-w 1)
            [x] (if (and x)
                  (ao :sr-sel x)))

     (when-let [imm-val
                (some #(match [%]
                              [(n :guard number?)] n
                              [(:or  [(:or :u :s) _ _])] %)
                      ((juxt :x :y :aluy) mc))]
       (ao :imm-val imm-val))

     (when-let [mac (:mac mc)]
       (gen-mac mac))
     (when (:mac-stall-sense mc)
       (so :mac-stall-sense))

     (if (:latch-s-mac mc)
       (so :mac-s-latch))
     (when-let [event (:event mc)]
       (case event
         :ack (so :event-ack-0)))
     (when (:ilevel-capture mc)
       (so :ilevel-cap))
     (when (:mask-int mc)
       (so :maskint-next))
     ;; memory access
     (when-let [ma (:ma mc)]
       (gen-ma ma))
     (when (:ma-lock mc)
       (so :mem-lock))

     (when-let [z (:z mc)]
       (z-bus z rn rm))
     (when (= :pc (:pr mc))
       (concat
        ["PR = PC"]
        (ao :wrpr-pc 1
            :wrreg-z 1
            :regnum-z (second (parser/register-map "pr")))))
     (when-let [w (:w mc)]
       (w-bus w rn rm))

     (when (= (:if-addr mc) :z)
       (so :ifadsel))
     (when (:inc-pc mc)
       (so :incpc))
     (when-let [dispatch (:dispatch mc)]
       (ao :dispatch dispatch))
     (when (:debug mc)
       (so :debug))
     (when-let [issue (:if-issue mc)]
       (ao :if-issue issue))
     (when (:delay-jump mc)
       (so :delay-jump))
     (when (:halt mc)
       (so :slp)))))

(defn gen-assigns-check [op mc]
  (let [assigns (gen-assigns op mc)]
    ;; ensure same signal isn't assigned multiple times.
    (doseq [[k num]
            (filter #(> (second %) 1)
                    (frequencies
                     (concat (map first (filter vector? assigns)))))]
      (throw (IllegalStateException. (str "control " k " is set " num " times"))))
    assigns))

(defn gen-assign-map
  ([op mc]
     (into {} (filter vector? (gen-assigns-check op mc)))))
