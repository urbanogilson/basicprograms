export const navLinks = [
    { id: "pull_requests", title: "Pull Requests", },
    { id: "explore", title: "Explore", },
    { id: "marketplace", title: "Marketplace", },
    { id: "issues", title: "Issues", },
];

export const folders = [
    {
        name: ".github",
        comment: "Moving the SDK into Arcade.",
        date: "6 days ago",
    },

    {
        name: "build",
        comment: "Get dotnet.Tests tests to run",
        date: "2 months ago",
    },

    { name: "documentation", comment: "fix line endings", date: "4 hours ago" },

    {
        name: "eng",
        comment:
            "updated CODEOWNERS file Add support for building 2.1 Razor projects using the RazorSDK",
        date: "14 months ago",
    },

    {
        name: "scripts",
        comment: "Hook up CI runs to run tests as tools",
        date: "12 days ago",
    },

    { name: "src", comment: "Adding tests", date: "5 days ago" },

    {
        name: ".editorconfig",
        comment: "Add MSBuild Logs to .gitignore",
        date: "3 years ago",
        file: true,
    },

    {
        name: ".gitattributes",
        comment: "remove build files from git",
        date: "21 days ago",
        file: true,
    },

    {
        name: "README.md",
        comment: "Adding tests",
        date: "21 days ago",
        file: true,
    },

    {
        name: "build.sh",
        comment: "Switch to 1ES R&D pools on main",
        date: "21 days ago",
        file: true,
    },

    {
        name: "global.json",
        comment: "updated CODEOWNERS file",
        date: "21 days ago",
        file: true,
    },

    {
        name: "test.sh",
        comment: "Link Code of Conduct",
        date: "21 days ago",
        file: true,
    },
];


export const readme = `## Welcome to dotnet sdk

This repo contains core functionality needed to create .NET projects that is shared between VisualStudio and CLI.

- MSBuild tasks can be found under [/src/Tasks/Microsoft.NET.Build.Tasks/](src/Tasks/Microsoft.NET.Build.Tasks).

Please refer to [dotnet/project-system](https://github.com/dotnet/project-system) for the project system work that is specific to Visual Studio.

## Build status

|                                                                                             Windows x64                                                                                             |
| :-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| [![](<https://dev.azure.com/dnceng/internal/_apis/build/status/dotnet/sdk/DotNet-Core-Sdk%203.0%20(Windows)%20(YAML)%20(Official)>)](https://dev.azure.com/dnceng/internal/_build?definitionId=140) |

## Installing the SDK

[Official builds](https://dotnet.microsoft.com/download/dotnet-core)

[Latest builds](https://github.com/dotnet/installer#installers-and-binaries)

## How do I engage and contribute?

We welcome you to try things out, [file issues](https://github.com/dotnet/sdk/issues), make feature requests and join us in design conversations. Also be sure to check out our [project documentation](documentation)

This project has adopted the [.NET Foundation Code of Conduct](https://dotnetfoundation.org/code-of-conduct) to clarify expected behavior in our community.

## How do I build the SDK?

Start with the [Developer Guide](documentation/project-docs/developer-guide.md).

## How do I test an SDK I have built?

To test your locally built SDK, run \`engdogfood.cmd\` after building. That script starts a new Powershell with the environment configured to redirect SDK resolution to your build.

From that shell your SDK will be available in:

- any Visual Studio instance launched (via \`& devenv.exe\`)
- \`dotnet build\`
- \`msbuild\`

## How we triage and review PRs

With the SDK repo being the home for many different areas, we've started trying to label incoming issues for the area they are related to using Area- labels. Then we rely on the [codeowners](https://github.com/dotnet/sdk/blob/main/CODEOWNERS) to manage and triages issues in their areas. Feel free to ping the owners listed in that file if you're not getting traction on a particular issue or PR. Please try to label new issues as that'll help us route them faster.

For PRs, we assign out a reviewer once a week on Wednesday looking only at PRs that are green in the build. If you are contributing, please get the PR green including a test if possible and then ping @dotnet-cli if you want to raise visibility of the PR.
`;