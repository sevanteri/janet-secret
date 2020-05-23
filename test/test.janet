(import secret)

(def schema
  {:name "test.schema"
   :flags :none
   :attributes {"string" :string
                "number" :integer
                "even" :boolean}})

(def attributes @{"string" "nine"
                  "number" 9
                  "even" false})

(let [res (secret/save-password
            schema
            attributes
            :default
            "test label"
            "secr3tpassword123")]
  (assert (= res true) "Couldn't save password"))

(let [pw (secret/lookup-password
           schema
            attributes)]
  (assert (= pw "secr3tpassword123") "Password didn't match"))

(let [res (secret/remove-password
            schema
            attributes)]
  (assert (= res true) "Couldn't remove password"))

(let [res (secret/remove-password
            schema
            attributes)]
  (assert (= res false) "Shouldn't be able to delete a non existent password"))

(let [pw (secret/lookup-password
           schema
            attributes)]
  (assert (nil? pw) "Password wasn't nil"))

