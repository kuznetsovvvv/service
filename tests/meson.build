deps = [
  dependency('gtest', fallback: ['gtest', 'gtest_dep']),
  dependency('gmock', fallback: ['gtest', 'gmock_dep']),
  # gmock_main initializes both gtest and gmock
  dependency('gmock_main', fallback: ['gtest', 'gmock_main_dep']),
  dependency('libpqxx'),
]
inc_dirs = [
  '../',
]
test_files = files('test.cpp')

extraarg = []

tests = executable('tests',
  test_files,
  install: false,
  dependencies: deps,
  include_directories: inc_dirs,
  override_options: extraarg,
)

test('Lab tests', tests)