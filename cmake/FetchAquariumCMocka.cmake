include(FetchContent)

FetchContent_Declare(
        aquarium-cmocka
        GIT_REPOSITORY https://github.com/pretore/aquarium-cmocka.git
        GIT_TAG v1.5.2
        GIT_SHALLOW 1
)

FetchContent_MakeAvailable(aquarium-cmocka)
