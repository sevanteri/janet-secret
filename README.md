# janet-secret

Bindings for libsecret.

Save and retrieve passwords and other secrets from your system's "Secret
Service" like gnome-keyring and ksecretservice.

Only synchronous functions are implemented for now.

# Install

  Packages needed are `libsecret-1-dev` and `libglib2.0-dev` (on Ubuntu).

  Then install this package:

    sudo jpm install https://github.com/sevanteri/janet-secret

# Examples

For more more details check the [libsecret manual](https://developer-old.gnome.org/libsecret/0.18/).

First you need a schema. Define it as a struct.

    (def schema
      {:name "test.schema"
       :flags :none # or :dont-match-name
       :attributes {"string" :string
                    "number" :integer
                    "even" :boolean}})

Then you can use that schema to save, lookup and remove a secret.

    (secret/save-password
      schema
      @{"string" "nine"
        "number" 9
        "even" false}
      :session # or :default
      "test label"
      "secr3tpassword123")

    (secret/lookup-password
      schema
      @{"string" "nine"
        "number" 9
        "even" false})

    (secret/remove-password
      schema
      @{"string" "nine"
        "number" 9
        "even" false})
