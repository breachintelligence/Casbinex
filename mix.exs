defmodule Mix.Tasks.Compile.Casbin do
  def run(_args) do

    case :os.type() do
      {:win32, _} ->
        "start"

      {:unix, :darwin} ->
        {result, _errcode} = System.cmd("/usr/bin/g++",
          ["--std=c++17",
          "-I", "c_src/include",
          "-I", "c_src/macos/include",
          "-I", "/usr/local/lib/erlang/usr/include/",
          "-I", "/usr/local/opt/libpq/include",
          "-fPIC",
          "-O2",
          "-Lc_src/macos/lib",
          "-L/usr/local/opt/libpq/lib",
          "-L/usr/local/opt/erlang/lib/erlang/lib",
          "-dynamiclib",
          "-o", "casbinex_nif.so",
          "c_src/casbin_nif.cpp",
          "c_src/pg_adapter.cpp",
          "c_src/macos/lib/casbin.a",
          "-lpqxx",
          "-lpq",
          "-flat_namespace",
          "-undefined", "suppress",

          ], stderr_to_stdout: true)
        IO.puts(result)

      {:unix, _} ->
        {result, _errcode} = System.cmd("/usr/bin/g++",
          ["--std=c++17",
            "-I", "c_src/include",
            "-I", "c_src/linux/include",
            "-fPIC",
            "-O2",
            "-Lc_src/linux/lib",
            "-shared",
            "-o", "casbinex_nif.so",
            "c_src/casbin_nif.cpp",
            "c_src/pg_adapter.cpp",
            "c_src/linux/lib/libpqxx.a",
            "c_src/linux/lib/casbin.a",
            "-lpq",
          ], stderr_to_stdout: true)
        IO.puts(result)
    end
  end
end

defmodule Casbinex.MixProject do
  use Mix.Project

  def project do
    [
      app: :casbinex,
      version: "0.1.0",
      elixir: "~> 1.12",
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      compilers: [:casbin] ++ Mix.compilers,
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"}
    ]
  end
end
