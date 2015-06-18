(ns cpugen.main
  (:require
   clojure.stacktrace
   [watchtower.core :as watch])
  (:use [clojure.tools.cli :only [cli]])
  (:gen-class))

(defn -main [& args]
  (let [[options args banner]
        (cli args
             ["-w" "--rom-width" "Number of bits in microcode ROM line (64 or 72)"
              :default 72 :parse-fn #(Integer. %)]
             ["-r" "--regen" "Watches the file for changes and reruns the generator when it changes"
              :default false :flag true]
             ["-h" "--help" "Show help" :default false :flag true])
        run-generator
        (fn []
          (apply (ns-resolve 'cpugen.core 'gen-decoder)
                 (mapcat identity options)))]
    (cond
     (:help options)
     (println banner)

     (not (#{64 72} (:rom-width options)))
     (do (println "Invalid ROM width")
         (println banner))

     (:regen options)
     (do
       (require 'cpugen.core)
       (let [file-name @(ns-resolve 'cpugen.core 'file-name)]
         (println "Watching" file-name)
         (watch/watcher
          ["."]
          (watch/rate 500)
          (watch/file-filter (fn [f] (= (.getName f) file-name)))
          (watch/on-change
           (fn [x]
             (try
               (println "Reading spreadsheet")
               (run-generator)
               (catch Exception e (clojure.stacktrace/print-stack-trace e))))))))

     :else
     (do
        (require 'cpugen.core)
        (run-generator)))))
