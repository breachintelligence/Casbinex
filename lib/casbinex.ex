defmodule Casbinex do
  @moduledoc """
  Documentation for `Casbinex`.
  """

  alias Casbinex.Base

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
      list -> list_to_strings(list)
    end
  end

  def getRolesForUser(user) do
    case Base.getRolesForUser(String.to_charlist(user)) do
      {:error, message} -> {:error, message}
      list -> list_to_strings(list)
    end
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
    Base.addRolesForUser(String.to_charlist(user), list_to_charlists(roles))
  end

  def addRolesForUser(user, roles) do
    Base.addRoleForUser(String.to_charlist(user), String.to_charlist(roles))
  end

  def getFilteredPolicy(fieldindex, fields) when is_list(fields) do
    case Base.getFilteredPolicy(fieldindex, list_to_charlists(fields)) do
      {:error, message} -> {:error, message}
      list -> map_to_stringlists(list)
    end
  end

  def getFilteredPolicy(fieldindex, field) do
    case Base.getFilteredPolicy(fieldindex, [String.to_charlist(field)]) do
      {:error, message} -> {:error, message}
      list -> map_to_stringlists(list)
    end
  end

  def removeFilteredPolicy(fieldindex, fields) when is_list(fields) do
    Base.removeFilteredPolicy(fieldindex, list_to_charlists(fields))
  end

  def removeFilteredPolicy(fieldindex, field) do
    Base.removeFilteredPolicy(fieldindex, [String.to_charlist(field)])
  end

  # Utility functions for converting lists of strings to charlists
  defp list_to_charlists(list) do
    list |> Enum.map(&to_charlist(&1))
  end

  defp list_to_strings(list) do
    list |> Enum.map(&List.to_string(&1))
  end

  defp map_to_stringlists(list) do
    list |> Enum.map(&list_to_strings(&1))
  end
end
