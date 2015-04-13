require File.join(File.dirname(__FILE__), 'helper')

class TestTuple
  def test_sort
    tuples = [
      [1, "foo"],
      [1, true],
      [2],
      [1],
      [nil],
      [true],
      [:foo, -1],
      [:foo, -2**64],
      [:foo,  2**64],
      [1, "foo", 7, nil, false, true],
      [1, "foo", 7, nil, false, false],
      ["charles", "atlas"],
      ["charles", "atlas", "shrugged"],
      ["charles", "atlantic"],
      ["charles", "atlas jr."],
      ["charles", "atlas", "world's", "strongest", "man"],
      ["charles", "atlas", 5],
      [now, "foo"],
      [now, "bar"],
      [now - 24 * 60 * 60],
      [today + 1],
      [today - 1],
      [today],
    ]

    expected = [
      [nil],
      [1],
      [1, "foo"],
      [1, "foo", 7, nil, false, false],
      [1, "foo", 7, nil, false, true],
      [1, true],
      [2],
      ["charles", "atlantic"],
      ["charles", "atlas"],
      ["charles", "atlas", 5],
      ["charles", "atlas", "shrugged"],
      ["charles", "atlas", "world's", "strongest", "man"],
      ["charles", "atlas jr."],
      [:foo, -18446744073709551616],
      [:foo, -1],
      [:foo, 18446744073709551616],
      [today - 1],
      [now - 24 * 60 * 60],
      [today],
      [now, "bar"],
      [now, "foo"],
      [today + 1],
      [true]
    ]

    assert_equal expected, tuples.sort_by {|t| Tuple.dump(t)}

    100.times do
      assert_equal expected, tuples.shuffle.sort_by {|t| Tuple.dump(t)}
    end
  end
end
