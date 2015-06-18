(ns cpugen.logic
  "Uses a java implementation of the Quine-McCluskey algorithm to
reduce logic that looks like (A and not(B) and C) or (A and C) or ..."
  (:require [clojure
             [string :as s]
             [set :as set]]
            [clojure.math.combinatorics :as combo])
  (:import [org.literateprograms.logic
            Formula Term]
           [de.upb.hni.vmagic.builtin
            StdLogic1164]
           [de.upb.hni.vmagic.literal
            StringLiteral]
           [de.upb.hni.vmagic.object
            ArrayElement])
  (:use cpugen.vmagic))

(defn term [& bits]
  (Term. (byte-array
          (map #(case %
                    0 (byte 0)
                    1 (byte 1)
                    nil Term/DontCare
                    \X Term/DontCare
                    \x Term/DontCare)
               
               bits))))

(defn to-term [t]
  (if (instance? Term t)
    t
    (apply term t)))

(defn formula [& terms]
  (Formula. (java.util.ArrayList.
             (map to-term terms))))

(defn reduce1 [^Formula f]
  (.reduceToPrimeImplicants f)
  f)

(defn reduce2 [^Formula f]
  (.reducePrimeImplicantsToSubset f)
  f)

(defn reduce-implicants [implicant-maps]
  (if (seq implicant-maps)
    (let [keys (distinct (mapcat keys implicant-maps))
          term-builder (fn [implicant]
                         (apply term (map (partial get implicant) keys)))
          terms (map term-builder implicant-maps)
          f (apply formula terms)]
      (reduce1 f)
      (reduce2 f)
      (map (fn [^Term term]
             (into {}
                   (filter
                    identity
                    (map #(if (= %2 Term/DontCare)
                            nil
                            [%1 (long %2)])
                         keys (.getVarVals term)))))
           (.getTermList f)))))

(defn str-to-logic-map [k s]
  (into {}
        (filter identity
                (map-indexed
                 (fn [i c]
                   (case c
                     \0 [[k i] 0]
                     \1 [[k i] 1]
                     nil))
                 (reverse
                  (s/replace s #" +" ""))))))

(defn logic-map-to-stdmatch [m & key-widths]
  (when (= 1 (mod (count key-widths) 2))
    (throw (IllegalArgumentException. "logic-map-to-stdmatch requires even number of arguments")))
  (s/join
   (mapcat
    (fn [[key width]]
      (map #(get m [key %] "-")
           (range (dec width) -1 -1)))
    (partition 2 key-widths))))

(defn- merge-conditions
  "Take a sequence of [index value] pairs sorted by index and all
  index are distinct. Merge values for consecutive indices into
  vectors and return sequence of [start-index [v1 v2 ..]] pairs."
  [conds]
  (first
   (reduce
    (fn [[ranges [start-i cur-range]] [[i v] [next-i _]]]
      ;;(println [ranges [start-i cur-range]] [[i v] [next-i _]])
      (if (= (inc i) next-i)
        ;; add to current range
        [ranges [start-i (conj cur-range v)]]
        ;; start new range
        [(conj ranges [start-i (conj cur-range v)]) [next-i []]]))
    [[] [(ffirst conds) []]]
    (partition 2 1 [[false 0]] conds))))

(defn logic-map-to-bit [m sigs]
  (when (seq m)
    (apply v-and
           (for [[k c] (group-by #(ffirst %) m)
                 :let [sig (get sigs k)]]
             (apply v-and
                    (map
                     (fn [[i v]]
                       (let [exp (if (= (.getType sig) std-logic)
                                   sig
                                   (ArrayElement. sig i))]
                         (case v
                           0 (v-not exp)
                           1 exp)))
                     (reverse (sort (map (fn [[[_ i] v]]
                                           [i v]) c)))))))))

(defn logic-map-to-bool [m sigs]
  (when (seq m)
    (apply v-and
           (for [[k c] (group-by #(ffirst %) m)
                 :let [sig (get sigs k)]]
             (apply v-and
                    (map
                     (fn [[i vals]]
                       (if (= 1 (count vals))
                         (v= (if (= (.getType sig) std-logic)
                               sig
                               (ArrayElement. sig i))
                             (case (first vals)
                               0 StdLogic1164/STD_LOGIC_0
                               1 StdLogic1164/STD_LOGIC_1))
                         (v=
                          (slice-downto sig (dec (+ i (count vals))) i)
                          (StringLiteral. (apply str (map str (reverse vals)))))))
                     (reverse (merge-conditions
                               (sort (map (fn [[[_ i] v]]
                                            [i v]) c))))))))))

(defn op-to-logic-map [op]
  (merge (str-to-logic-map :i (:op op))
         (str-to-logic-map :p (str (:plane op)))))

(defn logic-map-cost [lm]
  (count lm))

(defn intersects-sets? [a b]
  (let [a-only (set/difference a b)
        b-only (set/difference b a)]
    (not
     (seq
      (set/intersection
       (apply hash-set (map first a-only))
       (apply hash-set (map first b-only)))))))

(defn intersects? [a b]
  (intersects-sets? (apply hash-set a) (apply hash-set b)))

(defn relax-logic-set [logic-set constraint-sets]
  (let [;; determine the single bits that can be removed
        loose-bits
        (filter
         (fn [bit]
           (not (some (partial intersects-sets?
                               (disj logic-set bit)) constraint-sets)))
         logic-set)]
    (->> (combo/subsets loose-bits)
         ;;(combo/combinations logic-set (dec (count logic-set)))
         (map #(apply disj logic-set %))
         (filter (fn [subset]
                   (not (some (partial intersects-sets? subset) constraint-sets))))
         (sort-by count)
         first)))

(defn relax-logic-maps
  "Returns a logic map derived from the first argument. The returned
  logic map is relaxed by removing the maximum number of items that do
  not cause the result to overlap with any of the logic maps passed in
  as a sequence of constraint maps."
  [logic-maps]
  (let [logic-sets (apply hash-set (map #(apply hash-set %) logic-maps))]
    (vec (map
          (fn [logic-map]
            (let [logic-set (apply hash-set logic-map)
                  relax-set (relax-logic-set logic-set (disj logic-sets logic-set))]
              (if (= logic-set relax-set)
                (println "Not Relaxed:" logic-set)
                (println "Relaxed:" (set/difference logic-set relax-set)))
              (into {}
                    relax-set)))
          logic-maps))))
