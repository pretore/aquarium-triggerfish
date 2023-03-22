include(FetchContent)

FetchContent_Declare(
        aquarium-coral
        GIT_REPOSITORY https://github.com/pretore/aquarium-coral.git
        GIT_TAG v2.0.1
        GIT_SHALLOW 1
)

FetchContent_MakeAvailable(aquarium-coral)
