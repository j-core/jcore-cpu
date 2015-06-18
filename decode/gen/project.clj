(defproject cpugen "0.1.0-SNAPSHOT"
  :description "Code generator for producing parts of the CPU"
  :license {}
  :dependencies [[org.clojure/clojure "1.5.1"]
                 [org.clojure/data.csv "0.1.2"]
                 [org.clojure/core.match "0.2.0-alpha12"]
                 [org.clojure/math.combinatorics "0.0.4"]
                 [org.clojure/tools.cli "0.2.2"]
                 [org.apache.odftoolkit/simple-odf "0.7-incubating"]
                 [watchtower "0.1.1"]]
  :java-source-paths ["vmagic/src/main/java"
                      "logic"]
  :javac-options ["-target" "1.6" "-source" "1.6" "-Xlint:-options"]
  :main cpugen.main)
