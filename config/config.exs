use Mix.Config

config :casbinex,
  model_path: 'test/rbac_model.conf',
  pg_connection: 'postgresql://<user>:<pass>@<host>/<db>'


import_config "#{Mix.env()}.exs"
