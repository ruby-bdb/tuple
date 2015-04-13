require 'mkmf'

case RUBY_VERSION
when /\A1\.8/
  $CFLAGS += ' -DRUBY_1_8_x'
when /\A1\.9/
  $CFLAGS += ' -DRUBY_1_9_x'
when /\A2\.(0|1)/
  $CFLAGS += ' -DRUBY_2_x_x'
  $CFLAGS += ' -DRUBY_2_1_x'
when /\A2\.2/
  $CFLAGS += ' -DRUBY_2_x_x'
  $CFLAGS += ' -DRUBY_2_2_x'
else
  raise "unsupported Ruby version: #{RUBY_VERSION}"
end

create_makefile('tuple/tuple')
