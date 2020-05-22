(import build/secret :as secret)

(def schema
  {:name "test.schema"
   :flags secret/schema/NONE
   :attributes {"string" secret/attribute/STRING
                "number" secret/attribute/INTEGER
                "even" secret/attribute/BOOLEAN}})

(assert (= true
           (secret/save-password
             schema
             @{"string" "something"
               "number" "9"
               "even" "false"}
             secret/collection/DEFAULT
             "test label"
             "secr3tpassword123")))

(assert (= "secr3tpassword123"
           (secret/lookup-password
             schema
             @{"string" "something"
               "number" "9"
               "even" "false"})))
