(import build/secret :as secret)
(import test-schema)

(defn main [bin pw &opt label_ &]
  (print (secret/save-password
          test-schema/schema
          @{"token" "my_token"}
          secret/collection/DEFAULT
          (or label_ "fancy label")
          pw)))
