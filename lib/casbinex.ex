defmodule Casbinex do
  @moduledoc """
  Documentation for `Casbinex`.
  """

  @on_load :load_nifs

  app = Mix.Project.config[:app]
  def load_nifs do
    path = :filename.join(:code.priv_dir(unquote(app)), 'casbinex_nif')
    :ok = :erlang.load_nif(path, 0)
  end

  def createEnforcer(_modelpath, _policypath) do
    raise "NIF createEnforcerdd/2 not implemented"
  end

  def destroyEnforcer() do
    raise "NIF destroyEnforcer/0 not implemented"
  end

  def enforce(_sub, _obj, _act) do
    raise "NIF enforce/3 not implemented"
  end

  def addPolicy(_sub, _obj, _act) do
    raise "NIF addPolicy/3 not implemented"
  end

  def removePolicy(_sub, _obj, _act) do
    raise "NIF removePolicy/3 not implemented"
  end

  def addGroupingPolicy(_user, _group) do
    raise "NIF addGroupingPolicy/2 not implemented"
  end

  def getUsersForRole(_role) do
    raise "NIF getUsersForRole/1 not implemented"
  end

  def getRolesForUser(_user) do
    raise "NIF getRolesForUser/1 not implemented"
  end

  def deleteRole(_role) do
    raise "NIF deleteRole/1 not implemented"
  end

  def deleteRoleForUser(_user, _role) do
    raise "NIF deleteRoleForUser/2 not implemented"
  end

  def deleteRolesForUser(_user) do
    raise "NIF deleteRolesForUser/1 not implemented"
  end

  def addRoleForUser(_user, _role) do
    raise "NIF addRoleForUser/2 not implemented"
  end

  def addRolesForUser(_user, _role) do
    raise "NIF addRolesForUser/2 not implemented"
  end

  def getFilteredPolicy(_field_index, _fields) do
    raise "NIF getFilteredPolicy/2 not implemented"
  end

  def removeFilteredPolicy(_field_index, _fields) do
    raise "NIF removeFilteredPolicy/2 not implemented"
  end
end
