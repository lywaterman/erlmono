# PyEr

Library for calling python from Erlang.

## Dependencies:

Parts of the boost library, somewhat close to 1.48
Developement version of lua

These libraries easily can be obtained on ubuntu by running this:

`
sudo apt-get install libboost1.48-all-dev libpython2.7
`

## General usage:

Here an self-describing example of usage:

    {ok, VM} = moon:start_vm(). %% Beware! It spawns the os thread.
    {ok,_} = moon:eval(VM, "print 1").
    ok = moon:load(VM, "priv/test.py").
    moon:call(VM, test, test_function, [first_arg, <<"second_arg">>, {key, val, key2, val2]).
    ok = moon:stop_vm(VM).

Strictly speaking, moon:stop_vm/1 is used here just for symmetry.
VM will be stopped and freed when the erlang garbage collector detects that VM become a garbage.

## Type mapping:

<table>
  <tr>
    <th>Erlang</th>
    <th>Python</th>
    <th>Erlang</th>
    <th>Remarks</th>
  </tr>
  <tr>
    <td>none</td>
    <td>None</td>
    <td>none</td>
    <td>None in python</td>
  </tr>
  <tr>
    <td>true</td>
    <td>true</td>
    <td>true</td>
    <td>boolean in python</td>
  </tr>
  <tr>
    <td>false</td>
    <td>false</td>
    <td>false</td>
    <td>boolean in python</td>
  </tr>
  <tr>
    <td>42</td>
    <td>42</td>
    <td>42</td>
    <td>int in python</td>
  </tr>
  <tr>
    <td>42.123</td>
    <td>42.123</td>
    <td>42.123</td>
    <td>double in python</td>
  </tr>
  <tr>
    <td>atom</td>
    <td>"atom"</td>
    <td><<"atom">></td>
    <td>string in python, binary, when comes back to erlang</td>
  </tr>
  <tr>
    <td>"string"</td>
    <td>[115,116,114,105,110,103]</td>
    <td>"string"</td>
    <td>table with integers in string, dont use it!</td>
  </tr>
  <tr>
    <td><<"binary">></td>
    <td>"binary"</td>
    <td><<"binary">></td>
    <td>string in string</td>
  </tr>
  <tr>
    <td>[]</td>
    <td>[]</td>
    <td>[]</td>
    <td></td>
  </tr>
  <tr>
    <td>[10, 100, <<"abc">>]</td>
    <td>[10, 100, "abc"]</td>
    <td>[10, 100, <<"abc">>]</td>
    <td></td>
  </tr>
  <tr>
    <td>{yet, value, another, value}</td>
    <td>{yet="value", another="value"}</td>
    <td>{<<"another">>, <<"value">>, <<"yet">>, <<"value">>}</td>
  </tr>
</table>

## Todo:
* Get rid of libboost_thread dependency, and replace queue with just a mutex & condition variable
* Embed header-only part of boost to the build
* Convert erlang strings to lua strings properly
