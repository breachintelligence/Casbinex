defmodule Mix.Tasks.Compile.Casbin do

#TODO before release move compilation to makefile

  def archMatch("aarch64-" <> _) do "arm64" end
  def archMatch("arm64-" <> _) do "arm64" end
  def archMatch(_) do "x86_64" end

  def homebrewPath("arm64") do "/opt/homebrew" end
  def homebrewPath("x86_64") do "/usr/local" end

  def run(_args) do
    root_dir = :code.root_dir()
    erts_dir = Path.join(root_dir, "erts-#{:erlang.system_info(:version)}")
    erts_include_dir = Path.join(erts_dir, "include")
    sys_arch =  "#{:erlang.system_info(:system_architecture)}"
    machine_arch = archMatch(sys_arch)
    homebrew_prefix = homebrewPath(machine_arch)


    case :os.type() do
      {:win32, _} ->
        "start"

      {:unix, :darwin} ->
        {result, _errcode} = System.cmd("/usr/bin/g++",
          ["--std=c++17",
          "-I", "c_src/include",
          "-I", "c_src/macos/include",
          "-I", "#{homebrew_prefix}/lib/erlang/usr/include/",
          "-I", "#{homebrew_prefix}/opt/libpq/include",
          "-I", "#{homebrew_prefix}/opt/libpqxx/include",
          "-fPIC",
          "-O2",
          "-L#{homebrew_prefix}/opt/libpq/lib",
          "-L#{homebrew_prefix}/opt/libpqxx/lib",
          "-L#{homebrew_prefix}/opt/erlang/lib/erlang/lib",
          "-dynamiclib",
          "-o", load_nif_path(),
          "c_src/casbin_nif.cpp",
          "c_src/pg_adapter.cpp",
          "c_src/pg_pool.cpp",
          "c_src/macos/lib/#{machine_arch}/casbin.a",
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
            "-I", erts_include_dir,
            "-fPIC",
            "-O2",
            "-Lc_src/linux/lib/#{machine_arch}",
            "-shared",
            "-o", load_nif_path(),
            "c_src/casbin_nif.cpp",
            "c_src/pg_adapter.cpp",
            "c_src/pg_pool.cpp",
            "c_src/linux/lib/libpqxx.a",
            "c_src/linux/lib/casbin.a",
            "-lpq",
          ], stderr_to_stdout: true)
        IO.puts(result)
    end
  end

  defp load_nif_path() do
    with {:error, :bad_name} <- :code.priv_dir(:casbinex) do
      "../../_build/#{Mix.env()}/lib/casbinex/priv/casbinex_nif.so"
    else
      dir -> :filename.join(dir, "casbinex_nif.so")
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
