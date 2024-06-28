#!/usr/bin/ruby

require "../subprojects/cmock/lib/cmock"

raise 'Header file to mock & destination directory must be specified!' unless ARGV.length >= 2

cmock = CMock.new(:plugins => %i[ignore return_thru_ptr], :mock_prefix => 'mock_', :mock_path => ARGV[1])

cmock.setup_mocks(ARGV[0])
