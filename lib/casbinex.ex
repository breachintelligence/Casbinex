defmodule Casbinex do
  @moduledoc """
  Documentation for `Casbinex`.
  """

  alias Casbinex.Base

  def createEnforcer(modelpath, policypath) do
    Base.createEnforcer(modelpath, policypath)
  end

  def destroyEnforcer() do
    Base.destroyEnforcer()
  end

  def enforce(sub, obj, act) do
    Base.enforce(sub, obj, act)
  end

  def addPolicy(sub, obj, act) do
    Base.addPolicy(sub, obj, act)
  end

  def removePolicy(sub, obj, act) do
    Base.removePolicy(sub, obj, act)
  end

  def addGroupingPolicy(user, group) do
    Base.addGroupingPolicy(user, group)
  end

  def getUsersForRole(role) do
    Base.getUsersForRole(role)
  end

  def getRolesForUser(user) do
    Base.getRolesForUser(user)
  end

  def deleteRole(role) do
    Base.deleteRole(role)
  end

  def deleteRoleForUser(user, role) do
    Base.deleteRoleForUser(user, role)
  end

  def deleteRolesForUser(user) do
    Base.deleteRolesForUser(user)
  end

  def addRoleForUser(user, role) do
    Base.addRoleForUser(user, role)
  end

  def addRolesForUser(user, roles) when is_list(roles) do
    Base.addRolesForUser(user, roles)
  end

  def addRolesForUser(user, roles) do
    Base.addRoleForUser(user, roles)
  end

  def getFilteredPolicy(fieldindex, fields) when is_list(fields) do
    Base.getFilteredPolicy(fieldindex, fields)
  end

  def getFilteredPolicy(fieldindex, field) do
    Base.getFilteredPolicy(fieldindex, [field])
  end

  def removeFilteredPolicy(fieldindex, fields) when is_list(fields) do
    Base.removeFilteredPolicy(fieldindex, fields)
  end

  def removeFilteredPolicy(fieldindex, field) do
    Base.removeFilteredPolicy(fieldindex, [field])
  end
end
