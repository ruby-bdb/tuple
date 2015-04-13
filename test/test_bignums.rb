require File.join(File.dirname(__FILE__), 'helper')

class TestTuple
  def test_bignums
    t = [2**64, 2**38, 2**32, 2**32 - 1, 2**31, 2**31 - 1, 1, 0]
    t = t + t.reverse.collect {|n| -n}

    assert_equal t, Tuple.load(Tuple.dump(t))
    assert_equal t.reverse, t.sort_by {|i| Tuple.dump(i)}
  end
end
