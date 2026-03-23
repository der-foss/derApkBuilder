#!/usr/bin/env ruby

require 'optparse'
require 'fileutils'

options = {
  debug: false,
  install: false
}

OptionParser.new do |opts|
  opts.banner = "Usage: build.rb [options]"

  opts.on("-d", "--debug", "Build in Debug mode") do
    options[:debug] = true
  end

  opts.on("-i", "--install", "Install after build") do
    options[:install] = true
  end

  opts.on("-h", "--help", "Prints help") do
    puts opts
    exit 1
  end
end.parse!

build_type = options[:debug] ? "Debug" : "Release"

build_dir = ".build"
binary_name = "derapkbuilder"

system("cmake -B #{build_dir} -S . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE -DCMAKE_BUILD_TYPE=#{build_type}") or abort("cmake configure failed")

system("cmake --build #{build_dir}") or abort("build failed")

FileUtils.cp("#{build_dir}/compile_commands.json", ".") if File.exist?("#{build_dir}/compile_commands.json")

if File.exist?("#{build_dir}/#{binary_name}")
  FileUtils.mv("#{build_dir}/#{binary_name}", "./#{binary_name}", force: true)
else
  puts "warning: binary not found"
end

if options[:install]
  PREFIX = ENV["PREFIX"] || "/usr"
  install_path = "#{PREFIX}/bin/#{binary_name}"

  begin
    FileUtils.cp("./#{binary_name}", install_path)
    puts "Installed to #{install_path}"
  rescue Errno::EACCES
    puts "Permission denied. Try running with sudo."
  end
end