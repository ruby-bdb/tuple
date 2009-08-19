require File.dirname(__FILE__) + '/test_helper'

class TupleTest < Test::Unit::TestCase
  should "dump and load arrays of simple types" do
    t = [1, true, :foo, "foo", -1001, false, nil]
    assert_equal t, Tuple.load(Tuple.dump(t))
  end
  
  should "dump and load fixnums and bignums" do
    t = [2**64, 2**32, 2**32 - 1, 2**31, 2**31 - 1, 1, 0]
    t = t + t.collect {|n| -n}
    assert_equal t, Tuple.load(Tuple.dump(t))
  end

  should "convert single value into array" do
    assert_equal [1], Tuple.load(Tuple.dump(1))
  end

  should "sort tuples using binary" do
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
      [true]
    ]  
    assert_equal expected, tuples.sort_by {|t| Tuple.dump(t)}

    100.times do
      assert_equal expected, tuples.shuffle.sort_by {|t| Tuple.dump(t)}
    end
  end 
end
