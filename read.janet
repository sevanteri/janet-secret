(import build/secret :as secret)
(import test-schema)

(defn main [bin &opt pw label_ &]
  (print (secret/lookup-password
          test-schema/schema
          @{"name" "my token"
            "timestamp" 123})))
