use Mix.Config

config :casbinex,
  model_path: '/home/bill/Projects/casbinex/test/data/rbac_model.conf',
  pg_connection: 'postgresql://<user>:<pass>@<host>/<db>'


  import_config "#{Mix.env()}.exs"
