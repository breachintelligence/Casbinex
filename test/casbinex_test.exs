defmodule CasbinexTest do
  use ExUnit.Case
  # import_config "config.exs"

  @pgconnection Application.fetch_env!(:casbinex, :pg_connection)
  @model_path Application.fetch_env!(:casbinex, :model_path)

  doctest Casbinex

  test "enforces user policies correctly" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      # assert :true == Casbinex.enforce('alice', 'data1', 'read')
      assert :true == Casbinex.enforce('g:1', 'c:-1', 'read')
      assert :false == Casbinex.enforce('attacker', 'data1', 'read')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "enforces group policies correctly" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      # alice is in the data2_admin group
      assert :true == Casbinex.enforce('g:1', 'c:-1', 'read')
      assert :true == Casbinex.enforce('g:1', 'c:-1', 'write')
      assert :false == Casbinex.enforce('attacker', 'data2', 'read')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "get a list of users in the role" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      # alice is in the data2_admin group
      assert ['alice'] == Casbinex.getUsersForRole('data2_admin')

      #add bob to the group
      assert :true == Casbinex.addGroupingPolicy('bob', 'data2_admin')
      assert ['bob', 'alice'] == Casbinex.getUsersForRole('data2_admin')

      #delete the group
      assert :true == Casbinex.deleteRole('data2_admin')
      assert {:error, _} = Casbinex.getUsersForRole('data2_admin')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "get a list of roles for the user" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      assert ['data2_admin']  == Casbinex.getRolesForUser('alice')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "can add new policies" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      assert :true  == Casbinex.addPolicy('bill', 'data1', 'read')
      assert :true  == Casbinex.enforce('bill', 'data1', 'read')
      assert :false == Casbinex.enforce('bill', 'data1', 'write')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "can remove policies" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      assert :true  == Casbinex.enforce('alice', 'data1', 'read')
      assert :true  == Casbinex.removePolicy('alice', 'data1', 'read')
      assert :false == Casbinex.enforce('alice', 'data1', 'read')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "can remove user from all groups" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      assert :true  == Casbinex.enforce('alice', 'data2', 'read')
      assert :true  == Casbinex.deleteRolesForUser('alice')
      assert :false == Casbinex.enforce('alice', 'data2', 'read')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "can remove role for user" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      assert :true  == Casbinex.enforce('alice', 'data2', 'read')
      assert :true  == Casbinex.deleteRoleForUser('alice', 'data2_admin')
      assert :false == Casbinex.enforce('alice', 'data2', 'read')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "can add user to role" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      assert :true  == Casbinex.enforce('alice', 'data2', 'read')
      assert :true  == Casbinex.deleteRoleForUser('alice', 'data2_admin')
      assert :false == Casbinex.enforce('alice', 'data2', 'read')
      assert :true  == Casbinex.addRoleForUser('alice', 'data2_admin')
      assert :true == Casbinex.enforce('alice', 'data2', 'read')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "can add user to multiple roles" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      Casbinex.addPolicy('data1_admin', 'data1', 'write')

      assert :false == Casbinex.enforce('alice', 'data1', 'write')
      assert :true  == Casbinex.addRolesForUser('alice', ['data1_admin', 'data2_admin'])
      assert :true == Casbinex.enforce('alice', 'data2', 'write')
      assert :true == Casbinex.enforce('alice', 'data1', 'write')

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "retrieves a filtered list of policies" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      assert [
        ['bob', 'data2', 'write'],
        ['data2_admin', 'data2', 'write']] = Casbinex.getFilteredPolicy(1, ['data2', 'write'])

      assert :ok == Casbinex.destroyEnforcer();
  end

  test "removes a filtered list of policies" do
    assert { :ok, _ } = Casbinex.createEnforcer(@model_path, @pgconnection)

      assert [
        ['bob', 'data2', 'write'],
        ['data2_admin', 'data2', 'write']
        ] = Casbinex.getFilteredPolicy(1, ['data2', 'write'])

        Casbinex.removeFilteredPolicy(1, ['data2', 'write'])
        assert [] = Casbinex.getFilteredPolicy(1, ['data2', 'write'])

      assert :ok == Casbinex.destroyEnforcer();
  end
end
