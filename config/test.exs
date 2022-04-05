use Mix.Config

config :casbinex,
  model_path: "test/data/rbac_model.conf",
  pg_connection: "test/data/rbac_policy.csv" #postgresql://<user>:<pass>@<host>/<db>
