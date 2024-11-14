project('libpqxxtest',
        'cpp',
        default_options: {
          'cpp_std': 'c++17'
        }
       )
libpq = dependency('libpqxx')
boost = dependency('boost', modules: ['program_options'])

main = executable('main', 'main.cc', dependencies: [libpq, boost])
subdir('tests')