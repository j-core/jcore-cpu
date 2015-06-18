(ns cpugen.genc
  (:require [clojure.string :as s]))

(defn- op->mask [op]
  (let [mask
        (-> op
            (s/replace " " "")
            (s/replace "0" "1")
            (s/replace #"[^0-9]" "0"))
        match
        (-> op
            (s/replace " " "")
            (s/replace #"[^0-9]" "0"))]

    [(Integer/parseInt mask 2) (Integer/parseInt match 2)]))

(defn- gen-if-else [& cond-bodies]
  (let [n (count cond-bodies)]
    (case n
      0 ""
      1 (str "{\n  " (first cond-bodies) "\n}")
      2 (str "if (" (first cond-bodies) ") {\n  " (second cond-bodies) "\n}")
      (apply str (interpose " else " (map #(apply gen-if-else %) (partition 2 2 [] cond-bodies))))
      )))

(defn- gen-sprintf [op]
  (let [vars (re-seq #"Rn|Rm" (:name op))
        fmt (-> (:name op)
                (s/replace #"R[mn]" "R%hu"))
        args
        (case (:format op)
          :n {"Rn" "(instr >> 8) & 0xF"}
          :ni {"Rn" "(instr >> 8) & 0xF"}
          :m {"Rm" "(instr >> 8) & 0xF"}
          :nm {"Rn" "(instr >> 8) & 0xF"
               "Rm" "(instr >> 4) & 0xF"}
          :nd4 {"Rn" "(instr >> 4) & 0xF"}
          :nd8 {"Rn" "(instr >> 8) & 0xF"}
          :md {"Rm" "(instr >> 4) & 0xF"}
          :nmd {"Rn" "(instr >> 8) & 0xF"
                "Rm" "(instr >> 4) & 0xF"}
          {})]

    (str "snprintf(str, size, "
         "\"" fmt "\""
         (apply str (interleave (repeat ", ")
                                (->> vars
                                     (map
                                      #(get args % 0))
                                     (map
                                      #(str "(uint16_t)(" % ")")))))
         ")")))

(defn gen-line-fn [ops line]
  (str
   "static int line" line "(char *str, size_t size, uint16_t instr) {\n"

   (apply gen-if-else
          (concat (mapcat
                 (fn [op]
                   (let [[mask match] (op->mask (:op op))]
                     [(str "(instr & 0x" (Integer/toString mask 16) ") == 0x" (Integer/toString match 16))
                      (str ;;"/* " (:name op) " */\n"

                           "return " (gen-sprintf op) ";"
                           )]
                     ))
                 ops) ["return -1;"])
    )
   "\n"
   ;;(apply str (interpose \newline (map :name ops)))
   "}\n"
   ))

(defn gen-op-printer [ops]
  (let [ops
        (->> ops
             (filter #(= (:plane %) 0))
             ;;(filter #(= (:format %) :n))
             (map #(select-keys % [:op :name :format])))

        ops
        (map (fn [op]
               (let [line (first (s/split (:op op) #" "))
                     line (Integer/parseInt line 2)]
                 (assoc op :line line)))
             ops)
        ops-by-line (group-by :line ops)]

    (->>
     (concat
      ["#include \"sh2instr.h\""
       "#include <stdio.h>"]


      #_"int sign8(uint16_t i) {
  int x = (int) (i & 0xFF);
  int m = 1U << (8 - 1);
  return = (x ^ m) - m;
}"

      #_"int sign4(uint16_t i) {
  int x = (int) (i & 0xF);
  int m = 1U << (4 - 1);
  return = (x ^ m) - m;
}"

      (map #(gen-line-fn (ops-by-line %) %) (range 16))

      ["typedef int (*linefn)(char *, size_t, uint16_t);"
       "linefn line_fns[] = {"
       (apply str "  " (interpose ", " (map #(str "line" %) (range 16))))
       "};"
       "int op_name(char *str, size_t size, uint16_t instr) {
  snprintf(str, size, \"ERR\");
  linefn l = line_fns[(instr >> 12) & 0xF];
  return l(str, size, instr);
}

void print_instr(uint16_t instr) {
  char buf[256];
  op_name(buf, sizeof(buf), instr);
  printf(\"%s\", buf);
}
"]
      )
     (interpose \newline)
     (apply str))))
