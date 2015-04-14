require 'rake/testtask'
require "rake/extensiontask"

def gemspec
  @clean_gemspec ||= eval(File.read(File.expand_path('../tuple.gemspec', __FILE__)))
end

Rake::ExtensionTask.new('tuple', gemspec) do |ext|
  ext.cross_compile = true
  ext.cross_platform = ['x86-mingw32', 'x86-mswin32-60']
  ext.lib_dir = File.join('lib', 'tuple')

  # inject 1.8/1.9 pure-ruby entry point when cross compiling only
  ext.cross_compiling do |spec|
    spec.files << 'lib/tuple/tuple.rb'
  end

  # Clean compiled extension
  CLEAN.include "#{ext.lib_dir}/*.#{RbConfig::CONFIG['DLEXT']}"
end

Rake::TestTask.new(:test) do |test|
  test.pattern = 'test/**/test_*.rb'
end

desc "Compile and then run tests"
task :default => [:compile, :test]

task :clean do
  include FileUtils

  rm_rf("tmp")

  Dir["*/**/*"].each do |path|
    if path =~ /(\.o|\.bundle|\/Makefile)\z/
      rm_f(path)
    end
  end
end
