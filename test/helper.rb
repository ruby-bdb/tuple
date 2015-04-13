require 'rubygems'
require 'test/unit'
require 'tuple'

if not [].respond_to?(:shuffle)
  class Array
    def shuffle
      t_self = self.dup
      t_size = self.size
      result=[]
      t_size.times { result << t_self.slice!(rand(t_self.size)) }
      result
    end
  end
end

class Time
  def ==(other)
    # Ignore microseconds for testing.
    to_i == other.to_i
  end
end

class TestTuple < Test::Unit::TestCase
  def now
    @now ||= Time.now
  end

  def today
    @today ||= Date.today
  end
end
