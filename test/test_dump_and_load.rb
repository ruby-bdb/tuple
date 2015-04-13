require File.join(File.dirname(__FILE__), 'helper')

class TestTuple
  def test_dump_and_load
    t = [1, true, :foo, "foo", -1001, false, nil, now, today - 7, [:foo, 1, 4, nil]]
    assert_equal t, Tuple.load(Tuple.dump(t))
  end

  def test_single_value
    assert_equal [1], Tuple.load(Tuple.dump(1))
  end
end
