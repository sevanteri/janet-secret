(import build/secret :as secret)
(def schema
  {:name "test.schema"
   :flags secret/schema/NONE
   :attributes {"token" secret/attribute/STRING}})

