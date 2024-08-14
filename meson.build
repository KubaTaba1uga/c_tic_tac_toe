# Tic tac toe game written in C
# ******************************************************************************
# *    Project
# ******************************************************************************
project('c_tic_tac_toe', 'c',
         version: '0.0.1',
	 meson_version: '>=1.0.1',
	 default_options: [
            'werror=true',
            'c_std=c11',
         ],
)

# ******************************************************************************
# *    Logging
# ******************************************************************************
cmake = import('cmake')

opt_var = cmake.subproject_options()
# by default stumpless uses c11, which makes all implicit declaration to gnu library
#  compile errors. Overriding c11 to gnu11 fixes the errors.
opt_var.set_override_option('c_std', 'gnu11')
opt_var.set_override_option('werror', 'false')
opt_var.set_override_option('werror', 'false')
# opt_var.set_override_option('debug', 'true')
logging_proj = cmake.subproject('stumpless', options: opt_var)
logging_dep = logging_proj.dependency('stumpless')

# ******************************************************************************
# *    Multi-threading
# ******************************************************************************
# Declare a dependency on the pthread library
pthread_dep = dependency('threads')


# ******************************************************************************
# *    App
# ******************************************************************************
app_deps = [logging_dep, pthread_dep]
subdir('src')

main = executable('main', sources, dependencies: app_deps, include_directories: [join_paths('.', 'src')])

# ******************************************************************************
# *    Tests
# ******************************************************************************
subdir('test')
