(import build/secret :as _secret)


(def- schema-flag-map {:none _secret/schema/NONE
                       :dont-match-name _secret/schema/DONT-MATCH-NAME})


(def- attr-map {:string _secret/attribute/STRING
                :integer _secret/attribute/INTEGER
                :boolean _secret/attribute/BOOLEAN})


(defn- process-schema-attributes [attributes]
  (struct
    ;(-?>> attributes
           pairs
           (map (fn [[key value]] [key (get attr-map value _secret/attribute/STRING)]))
           flatten)))


(defn- process-schema [schema]
  (let [name (get schema :name)
        flags (get schema :flags)
        attrs (get schema :attributes {})]

      (if (nil? name)
          (error "Schema needs a name"))

      {:name name
       :flags (get schema-flag-map flags _secret/schema/NONE)
       :attributes (process-schema-attributes attrs)}))


(defn lookup-password [schema attributes]
  # Turn attributes to strings
  # libsecret/secret-password.c comment:
  # "The @attributes should be a set of key and value string pairs."
  (let [attr (table ;(map string (kvs attributes)))
        schema_ (process-schema schema)]
    (_secret/lookup-password schema_
                             attr)))


(defn save-password [schema attributes collection
                     label_ password]
  # Turn attributes to strings
  # libsecret/secret-password.c comment:
  # "The @attributes should be a set of key and value string pairs."
  (let [attr (table ;(map string (kvs attributes)))
        schema (process-schema schema)]
    (_secret/save-password schema
                           attr
                           (string collection)
                           label_
                           password)))


(defn remove-password [schema attributes]
  # Turn attributes to strings
  # libsecret/secret-password.c comment:
  # "The @attributes should be a set of key and value string pairs."
  (let [attr (table ;(map string (kvs attributes)))
        schema_ (process-schema schema)]
    (_secret/remove-password schema_
                             attr)))


