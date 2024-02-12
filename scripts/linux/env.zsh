BUILD_TYPE="RelWithDebInfo"
SCRIPT_DIR=$(dirname "$0")
ABS_SCRIPT_DIR=$(cd ${SCRIPT_DIR} && pwd)
ARTIFACT_DIR=$(cd "${ABS_SCRIPT_DIR}/../../modules/build/${BUILD_TYPE}" && pwd)

so_files=($(find ${ARTIFACT_DIR} -type f -name "*.so"))

declare -A pybind_module_paths=()
declare -A lib_paths=()

for so_file in "${so_files[@]}"; do
  abs_so_file=$(realpath "$so_file")
  so_file_path=$(dirname "$abs_so_file")
  so_file_name=$(basename "$abs_so_file")
  if [[ $so_file_name == *"cpython"* ]]; then
    pybind_module_paths[$so_file_path]=$so_file_path
  else
    lib_paths[$so_file_path]=$so_file_path
  fi
done

python_path=""

for key in "${(@k)pybind_module_paths}"; do
  python_path="${python_path}:${pybind_module_paths[${key}]}"
done

export PYTHONPATH=${PYTHONPATH}${python_path}

ld_path=""

for key in "${(@k)lib_paths}"; do
  ld_path="${ld_path}:${lib_paths[${key}]}"
done

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}${ld_path}