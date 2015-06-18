(ns cpugen.vcd
  (:require [clojure.string :as s]
            clojure.stacktrace
            clojure.java.io)
  (:use [clojure.core.match :only (match)]))

(defn read-vcd [file-name]
  (with-open [rdr (clojure.java.io/reader file-name)]
    (let [[hdr dump]
          (split-with #(not= "#0" %) (line-seq rdr))
          wire-map
          (->> hdr
               (map
                #(when-let [[_ length ident name]
                            (re-matches #"\$var +wire +([0-9]+) +([^ ]+) +([^ ]+) +\$end" %)]
                   [ident {:name name :length (Long/parseLong length)}]))
               (filter identity)
               (into {}))
          time-test (fn [^String s] (.startsWith s "#"))
          time-vals
          (->> dump
               (partition-by time-test)
               (partition 2 1)
               (filter (fn [[[a] [b]]]
                         (and (time-test a) (not (time-test b)))))
               (mapv (fn [[[t] wire-vals]]
                       {:time (Long/parseLong (.substring t 1))
                        :vals
                        (->> wire-vals
                             (map #(when-let
                                       [[_ v n]
                                        (re-matches #"b?([01]+) *(n[0-9]+)" %)]
                                     (if (wire-map n)
                                       [n (Long/parseLong v 2)]
                                       (println "unknown wire" n))))
                             (filter identity)
                             (into {}))})))]
;;      (println wire-map)
      #_(doseq [t time-vals]
        (println "TIME=" (:time t) "num vals" (count (:vals t))))
      #_(println (count a) (count b))
      {:wires wire-map
       :time-vals time-vals})))
