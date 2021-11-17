defmodule Mix.Tasks.Compile.Casbin do
  def run(_args) do
    {result, _errcode} = System.cmd("make", ["priv/casbinex_nif.so"], stderr_to_stdout: true)
    IO.puts(result)
  end
end

defmodule Casbinex.MixProject do
  use Mix.Project

  def project do
    [
      app: :casbinex,
      version: "0.1.0",
      elixir: "~> 1.3",
      build_embedded: Mix.env == :prod,
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      compilers: [:casbin] ++ Mix.compilers,
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger],
      applications: [:logger]
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
