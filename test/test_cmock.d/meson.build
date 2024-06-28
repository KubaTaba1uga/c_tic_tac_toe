test_cmock_name = 'test_cmock.c'

cmock_test_src = files([
  test_cmock_name
])

test_cmock_exe = executable('test_cmock',
  sources: [
   cmock_test_src,
   cmock_gen_runner.process(test_cmock_name)                   
  ],
  dependencies: [unity_dependency],
)

test('test_cmock_installation', test_cmock_exe, priority: 999)
