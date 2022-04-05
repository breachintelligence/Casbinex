defmodule Casbinex do
  @moduledoc """
  Documentation for `Casbinex`.
  """

  alias Casbinex.Base

  defp listToCharlists(list) do
    list |> Enum.map(&to_charlist(&1))
  end

  defp listToStrings(list) do
    list |> Enum.map(&List.to_string(&1))
  end

  defp listsToStrings(list) do
    list |> Enum.map(&listToStrings(&1))
  end

  def createEnforcer(modelpath, policypath) do
    Base.createEnforcer(String.to_charlist(modelpath), String.to_charlist(policypath))
  end

  def destroyEnforcer() do
    Base.destroyEnforcer()
  end

  def enforce(sub, obj, act) do
    Base.enforce(String.to_charlist(sub), String.to_charlist(obj), String.to_charlist(act))
  end

  def addPolicy(sub, obj, act) do
    Base.addPolicy(String.to_charlist(sub), String.to_charlist(obj), String.to_charlist(act))
  end

  def removePolicy(sub, obj, act) do
    Base.removePolicy(String.to_charlist(sub), String.to_charlist(obj), String.to_charlist(act))
  end

  def addGroupingPolicy(user, group) do
    Base.addGroupingPolicy(String.to_charlist(user), String.to_charlist(group))
  end

  def getUsersForRole(role) do
    case Base.getUsersForRole(String.to_charlist(role)) do
      {:error, message} -> {:error, message}
      list -> listToStrings(list)
    end
  end

  def getRolesForUser(user) do
    Base.getRolesForUser(String.to_charlist(user))
    |> listToStrings
  end

  def deleteRole(role) do
    Base.deleteRole(String.to_charlist(role))
  end

  def deleteRoleForUser(user, role) do
    Base.deleteRoleForUser(String.to_charlist(user), String.to_charlist(role))
  end

  def deleteRolesForUser(user) do
    Base.deleteRolesForUser(String.to_charlist(user))
  end

  def addRoleForUser(user, role) do
    Base.addRoleForUser(String.to_charlist(user), String.to_charlist(role))
  end

  def addRolesForUser(user, roles) when is_list(roles) do
    Base.addRolesForUser(String.to_charlist(user), listToCharlists(roles))
  end

  def addRolesForUser(user, roles) do
    Base.addRoleForUser(String.to_charlist(user), String.to_charlist(roles))
  end

  def getFilteredPolicy(fieldindex, fields) when is_list(fields) do
    Base.getFilteredPolicy(fieldindex, listToCharlists(fields))
    |> listsToStrings
  end

  def getFilteredPolicy(fieldindex, field) do
    Base.getFilteredPolicy(fieldindex, [String.to_charlist(field)])
    |> listsToStrings
  end

  def removeFilteredPolicy(fieldindex, fields) when is_list(fields) do
    Base.removeFilteredPolicy(fieldindex, listToCharlists(fields))
  end

  def removeFilteredPolicy(fieldindex, field) do
    Base.removeFilteredPolicy(fieldindex, [String.to_charlist(field)])
  end
end
