# Continuous Integration (CI) Test for Board Support Pack (BSP)

Content of `.ci` Directory     | Description
:------------------------------|:-----------------
`vcpkg-configuration.json`     | Tool setup for the CI test with AC6, GCC, CLANG
`vcpkg-configuration-AC6.json` | Tool setup for the CI test with AC6
`sdsio.yml`                    | FVP configuration for ST B-U585I-IOT02A sdsio-vsi interface

The [GitHub Actions](https://github.com/Arm-Examples/SDS-Examples/tree/main/README.md#github-actions) in the directory [`.github/workflows`](https://github.com/Arm-Examples/SDS-Examples/tree/main/.github/workflows) are the scripts for the CI tests. These scripts contain detailed comments about each step that is executed.
