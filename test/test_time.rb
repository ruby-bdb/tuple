require File.join(File.dirname(__FILE__), 'helper')

class TestTuple
  def test_time
    t = '2009-10-15 1:23:45 PM'
    tuple = Tuple.dump(Time.parse(t))

    10000.times do
      assert_equal tuple, Tuple.dump(Time.parse(t))
    end
  end
end
