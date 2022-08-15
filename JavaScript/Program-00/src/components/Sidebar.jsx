import React from "react";

function Sidebar() {
  return (
    <div className="sidebar w-1/4">
      <div className="about">
        <h4 className="text-base font-semibold">About</h4>
        <p className="text-base mt-4">
          Core functionality needed to create .NET Core projects, that is shared
          between Visual Studio and CLI
        </p>
        <div className="about-url flex items-center mt-4">
          <svg
            aria-hidden="true"
            viewBox="0 0 16 16"
            data-view-component="true"
            class="mr-2 w-4"
          >
            <path
              fill-rule="evenodd"
              d="M7.775 3.275a.75.75 0 001.06 1.06l1.25-1.25a2 2 0 112.83 2.83l-2.5 2.5a2 2 0 01-2.83 0 .75.75 0 00-1.06 1.06 3.5 3.5 0 004.95 0l2.5-2.5a3.5 3.5 0 00-4.95-4.95l-1.25 1.25zm-4.69 9.64a2 2 0 010-2.83l2.5-2.5a2 2 0 012.83 0 .75.75 0 001.06-1.06 3.5 3.5 0 00-4.95 0l-2.5 2.5a3.5 3.5 0 004.95 4.95l1.25-1.25a.75.75 0 00-1.06-1.06l-1.25 1.25a2 2 0 01-2.83 0z"
            ></path>
          </svg>
          <a className="text-blue-600 hover:underline font-semibold" href="¤">
            dot.net/core
          </a>
        </div>
        <div className="tag text-xs font-semibold flex items-center flex-wrap mt-2">
          <a
            className="rounded-full bg-blue-100 px-2 py-1 text-blue-600 hover:text-white hover:bg-blue-600 mr-1  mt-2"
            href="¤"
          >
            cli
          </a>
          <a
            className="rounded-full bg-blue-100 px-2 py-1 text-blue-600 hover:text-white hover:bg-blue-600 mr-1  mt-2"
            href="¤"
          >
            visual-studio
          </a>
          <a
            className="rounded-full bg-blue-100 px-2 py-1 text-blue-600 hover:text-white hover:bg-blue-600 mr-1  mt-2"
            href="¤"
          >
            sdk
          </a>
          <a
            className="rounded-full bg-blue-100 px-2 py-1 text-blue-600 hover:text-white hover:bg-blue-600  mr-1 mt-2"
            href="¤"
          >
            dotnet
          </a>
        </div>
        <div className="mt-4 text-gray-700">
          <a className="flex items-center mt-2 hover:text-blue-600" href="¤">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 mr-3 fill-current "
            >
              <path
                fill-rule="evenodd"
                d="M0 1.75A.75.75 0 01.75 1h4.253c1.227 0 2.317.59 3 1.501A3.744 3.744 0 0111.006 1h4.245a.75.75 0 01.75.75v10.5a.75.75 0 01-.75.75h-4.507a2.25 2.25 0 00-1.591.659l-.622.621a.75.75 0 01-1.06 0l-.622-.621A2.25 2.25 0 005.258 13H.75a.75.75 0 01-.75-.75V1.75zm8.755 3a2.25 2.25 0 012.25-2.25H14.5v9h-3.757c-.71 0-1.4.201-1.992.572l.004-7.322zm-1.504 7.324l.004-5.073-.002-2.253A2.25 2.25 0 005.003 2.5H1.5v9h3.757a3.75 3.75 0 011.994.574z"
              ></path>
            </svg>
            <span>Readme</span>
          </a>
          <a className="flex items-center mt-2 hover:text-blue-600" href="¤">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 mr-3 fill-current "
            >
              <path
                fill-rule="evenodd"
                d="M8.75.75a.75.75 0 00-1.5 0V2h-.984c-.305 0-.604.08-.869.23l-1.288.737A.25.25 0 013.984 3H1.75a.75.75 0 000 1.5h.428L.066 9.192a.75.75 0 00.154.838l.53-.53-.53.53v.001l.002.002.002.002.006.006.016.015.045.04a3.514 3.514 0 00.686.45A4.492 4.492 0 003 11c.88 0 1.556-.22 2.023-.454a3.515 3.515 0 00.686-.45l.045-.04.016-.015.006-.006.002-.002.001-.002L5.25 9.5l.53.53a.75.75 0 00.154-.838L3.822 4.5h.162c.305 0 .604-.08.869-.23l1.289-.737a.25.25 0 01.124-.033h.984V13h-2.5a.75.75 0 000 1.5h6.5a.75.75 0 000-1.5h-2.5V3.5h.984a.25.25 0 01.124.033l1.29.736c.264.152.563.231.868.231h.162l-2.112 4.692a.75.75 0 00.154.838l.53-.53-.53.53v.001l.002.002.002.002.006.006.016.015.045.04a3.517 3.517 0 00.686.45A4.492 4.492 0 0013 11c.88 0 1.556-.22 2.023-.454a3.512 3.512 0 00.686-.45l.045-.04.01-.01.006-.005.006-.006.002-.002.001-.002-.529-.531.53.53a.75.75 0 00.154-.838L13.823 4.5h.427a.75.75 0 000-1.5h-2.234a.25.25 0 01-.124-.033l-1.29-.736A1.75 1.75 0 009.735 2H8.75V.75zM1.695 9.227c.285.135.718.273 1.305.273s1.02-.138 1.305-.273L3 6.327l-1.305 2.9zm10 0c.285.135.718.273 1.305.273s1.02-.138 1.305-.273L13 6.327l-1.305 2.9z"
              ></path>
            </svg>
            <span> MIT license</span>
          </a>
          <a className="flex items-center mt-2 hover:text-blue-600" href="¤">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 mr-3 fill-current "
            >
              <path
                fill-rule="evenodd"
                d="M8.048 2.241c.964-.709 2.079-1.238 3.325-1.241a4.613 4.613 0 013.282 1.355c.41.408.757.86.996 1.428.238.568.348 1.206.347 1.968 0 2.193-1.505 4.254-3.081 5.862-1.496 1.526-3.213 2.796-4.249 3.563l-.22.163a.75.75 0 01-.895 0l-.221-.163c-1.036-.767-2.753-2.037-4.249-3.563C1.51 10.008.007 7.952.002 5.762a4.614 4.614 0 011.353-3.407C3.123.585 6.223.537 8.048 2.24zm-1.153.983c-.81.78-1.546 1.669-2.166 2.417-.184.222-.358.432-.52.623a.75.75 0 00.04 1.016c.35.35.697.697 1.043 1.047.866.875 2.292.914 3.185.032.264-.26.534-.528.802-.797.694-.694 1.8-.701 2.474-.03L12.92 8.7l.283.284c-.244.334-.515.666-.81.995l-1.384-1.28A.75.75 0 109.99 9.802l1.357 1.252c-.325.31-.656.606-.984.887l-1.48-1.366a.75.75 0 10-1.018 1.102L9.191 12.9c-.433.34-.838.643-1.191.905-1.04-.773-2.537-1.907-3.846-3.242C2.611 8.99 1.502 7.306 1.502 5.75a3.114 3.114 0 01.913-2.335c1.159-1.158 3.23-1.224 4.48-.191zm7.112 4.442c.313-.65.491-1.293.491-1.916v-.001c0-.614-.088-1.045-.23-1.385-.143-.339-.357-.633-.673-.949a3.113 3.113 0 00-2.218-.915c-1.092.003-2.165.627-3.226 1.602-.823.755-1.554 1.637-2.228 2.45l-.127.154.562.566a.756.756 0 001.066.02l.794-.79c1.258-1.258 3.312-1.31 4.594-.032.396.394.792.791 1.173 1.173l.022.023z"
              ></path>
            </svg>
            <span>Code of conduct</span>
          </a>
          <a className="flex items-center mt-2 hover:text-blue-600" href="¤">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 mr-3 fill-current "
            >
              <path
                fill-rule="evenodd"
                d="M8 .25a.75.75 0 01.673.418l1.882 3.815 4.21.612a.75.75 0 01.416 1.279l-3.046 2.97.719 4.192a.75.75 0 01-1.088.791L8 12.347l-3.766 1.98a.75.75 0 01-1.088-.79l.72-4.194L.818 6.374a.75.75 0 01.416-1.28l4.21-.611L7.327.668A.75.75 0 018 .25zm0 2.445L6.615 5.5a.75.75 0 01-.564.41l-3.097.45 2.24 2.184a.75.75 0 01.216.664l-.528 3.084 2.769-1.456a.75.75 0 01.698 0l2.77 1.456-.53-3.084a.75.75 0 01.216-.664l2.24-2.183-3.096-.45a.75.75 0 01-.564-.41L8 2.694v.001z"
              ></path>
            </svg>
            <span>
              <span className="font-semibold">1.9k </span>stars
            </span>
          </a>
          <a className="flex items-center mt-2 hover:text-blue-600" href="¤">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 mr-3 fill-current "
            >
              <path
                fill-rule="evenodd"
                d="M1.679 7.932c.412-.621 1.242-1.75 2.366-2.717C5.175 4.242 6.527 3.5 8 3.5c1.473 0 2.824.742 3.955 1.715 1.124.967 1.954 2.096 2.366 2.717a.119.119 0 010 .136c-.412.621-1.242 1.75-2.366 2.717C10.825 11.758 9.473 12.5 8 12.5c-1.473 0-2.824-.742-3.955-1.715C2.92 9.818 2.09 8.69 1.679 8.068a.119.119 0 010-.136zM8 2c-1.981 0-3.67.992-4.933 2.078C1.797 5.169.88 6.423.43 7.1a1.619 1.619 0 000 1.798c.45.678 1.367 1.932 2.637 3.024C4.329 13.008 6.019 14 8 14c1.981 0 3.67-.992 4.933-2.078 1.27-1.091 2.187-2.345 2.637-3.023a1.619 1.619 0 000-1.798c-.45-.678-1.367-1.932-2.637-3.023C11.671 2.992 9.981 2 8 2zm0 8a2 2 0 100-4 2 2 0 000 4z"
              ></path>
            </svg>
            <span>
              <span className="font-semibold">213 </span>watching
            </span>
          </a>
          <a className="flex items-center mt-2 hover:text-blue-600" href="¤">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 mr-3 fill-current "
            >
              <path
                fill-rule="evenodd"
                d="M5 3.25a.75.75 0 11-1.5 0 .75.75 0 011.5 0zm0 2.122a2.25 2.25 0 10-1.5 0v.878A2.25 2.25 0 005.75 8.5h1.5v2.128a2.251 2.251 0 101.5 0V8.5h1.5a2.25 2.25 0 002.25-2.25v-.878a2.25 2.25 0 10-1.5 0v.878a.75.75 0 01-.75.75h-4.5A.75.75 0 015 6.25v-.878zm3.75 7.378a.75.75 0 11-1.5 0 .75.75 0 011.5 0zm3-8.75a.75.75 0 100-1.5.75.75 0 000 1.5z"
              ></path>
            </svg>
            <span>
              <span className="font-semibold">821 </span>forks
            </span>
          </a>
        </div>
        <div className="releases border-t-2 mt-4">
          <a className="flex items-center group mt-4" href="¤">
            <h4 className="text-base group-hover:text-blue-600 font-semibold">
              Releases
              <span className="rounded-full bg-gray-200 font-semibold text-xs px-2 ml-1">
                99
              </span>
            </h4>
          </a>
          <a href="¤" className="mt-4 flex group">
            <svg
              aria-hidden="true"
              height="16"
              viewBox="0 0 16 16"
              version="1.1"
              width="16"
              data-view-component="true"
              class="w-4 fill-current mt-1 text-green-700 group-hover:text-blue-600"
            >
              <path
                fill-rule="evenodd"
                d="M2.5 7.775V2.75a.25.25 0 01.25-.25h5.025a.25.25 0 01.177.073l6.25 6.25a.25.25 0 010 .354l-5.025 5.025a.25.25 0 01-.354 0l-6.25-6.25a.25.25 0 01-.073-.177zm-1.5 0V2.75C1 1.784 1.784 1 2.75 1h5.025c.464 0 .91.184 1.238.513l6.25 6.25a1.75 1.75 0 010 2.474l-5.026 5.026a1.75 1.75 0 01-2.474 0l-6.25-6.25A1.75 1.75 0 011 7.775zM6 5a1 1 0 100 2 1 1 0 000-2z"
              ></path>
            </svg>
            <span className="ml-2">
              <span className="font-semibold group-hover:text-blue-600">
                .NET SDK 6.0.302
              </span>
              <span className="border border-green-700 text-xs font-semibold rounded-full text-green-700 ml-2 inline-block px-2 ">
                Latest
              </span>
              <span className="block text-xs group-hover:text-blue-600">
                28 days ago
              </span>
            </span>
          </a>
          <a
            className="my-4 flex items-center text-blue-600 hover:underline"
            href="¤"
          >
            + 98 releases
          </a>
        </div>
        <div className="packages border-t-2 mt-4">
          <a className="flex items-center group mt-4" href="¤">
            <h4 className="text-base group-hover:text-blue-600 font-semibold">
              Packages
            </h4>
          </a>
          <span className="my-4 text-xs flex items-center text-gray-700">
            No packages published
          </span>
        </div>

        <div className="contributers border-t-2 mt-4">
          <a className="flex items-center group mt-4" href="¤">
            <h4 className="text-base group-hover:text-blue-600 font-semibold">
              Contributors
              <span className="rounded-full bg-gray-200 font-semibold text-xs px-2 ml-1">
                420
              </span>
            </h4>
          </a>
          <div className="flex items-center flex-wrap mt-2">
            <img
              className="w-7 rounded-full mt-1 mr-2"
              src="https://avatars.githubusercontent.com/in/16706?s=64"
              alt="avatar"
            />
            <img
              className="w-8 rounded-full  mt-1  mr-2"
              src="https://avatars.githubusercontent.com/u/46013274?s=64"
              alt="avatar"
            />
            <img
              className="w-8 rounded-full  mt-1  mr-2"
              src="https://avatars.githubusercontent.com/u/3981619?s=64&v=4"
              alt="avatar"
            />
            <img
              className="w-8 rounded-full  mt-1  mr-2"
              src="https://avatars.githubusercontent.com/u/75470?s=64&v=4"
              alt="avatar"
            />
            <img
              className="w-8 rounded-full  mt-1  mr-2"
              src="https://avatars.githubusercontent.com/u/367554?s=64&v=4"
              alt="avatar"
            />
            <img
              className="w-8 rounded-full  mt-1  mr-2"
              src="https://avatars.githubusercontent.com/u/16010855?s=64&v=4"
              alt="avatar"
            />
            <img
              className="w-8 rounded-full  mt-1  mr-2"
              src="https://avatars.githubusercontent.com/u/3981619?s=64&v=4"
              alt="avatar"
            />
            <img
              className="w-8 rounded-full  mt-1  mr-2"
              src="https://avatars.githubusercontent.com/u/2379589?s=64&v=4"
              alt="avatar"
            />
          </div>
          <a
            className="my-4 flex items-center text-blue-600 hover:underline"
            href="¤"
          >
            + 409 contributors
          </a>
        </div>
        <div className="languages border-t-2 mt-4">
          <h4 className="text-base font-semibold mt-4">Languages</h4>
          <div className="flex mt-4">
            <div className="w-10/12 h-2 rounded-md rounded-r-none bg-green-700"></div>
            <div className="w-1/12 h-2 bg-blue-900"></div>
            <div className="w-1 h-2 bg-green-200"></div>
            <div className="w-1 h-2 bg-red-400"></div>
            <div className="w-1 h-2 bg-yellow-300"></div>
            <div className="w-1 h-2 rounded-md rounded-l-none bg-red-700"></div>
          </div>
          <ul className="mt-2 flex flex-wrap items-center text-xs">
            <li className="flex items-center mr-5">
              <a className="flex items-center group" href="¤">
                <span className="w-2 h-2 bg-green-700 rounded-full"></span>
                <span className="ml-3 group-hover:text-blue-600">
                  <span className="font-semibold">C#</span>
                  <span className="ml-1">96.9%</span>
                </span>
              </a>
            </li>
            <li className="flex items-center  mr-5">
              <a className="flex items-center group" href="¤">
                <span className="w-2 h-2 bg-blue-900 rounded-full"></span>
                <span className="ml-3 group-hover:text-blue-600">
                  <span className="font-semibold">PowerShell</span>
                  <span className="ml-1">96.9%</span>
                </span>
              </a>
            </li>
            <li className="flex items-center mr-5">
              <a className="flex items-center group" href="¤">
                <span className="w-2 h-2 bg-blue-200 rounded-full"></span>
                <span className="ml-3 group-hover:text-blue-600">
                  <span className="font-semibold">Shell</span>
                  <span className="ml-1">96.9%</span>
                </span>
              </a>
            </li>
            <li className="flex items-center mr-5">
              <a className="flex items-center group" href="¤">
                <span className="w-2 h-2 bg-red-400 rounded-full"></span>
                <span className="ml-3 group-hover:text-blue-600">
                  <span className="font-semibold">HTML</span>
                  <span className="ml-1">96.9%</span>
                </span>
              </a>
            </li>
            <li className="flex items-center mr-5">
              <a className="flex items-center group" href="¤">
                <span className="w-2 h-2 bg-yellow-300 rounded-full"></span>
                <span className="ml-3 group-hover:text-blue-600">
                  <span className="font-semibold">JavaScript</span>
                  <span className="ml-1">96.9%</span>
                </span>
              </a>
            </li>
            <li className="flex items-center mr-5">
              <a className="flex items-center group" href="¤">
                <span className="w-2 h-2 bg-red-700 rounded-full"></span>
                <span className="ml-3 group-hover:text-blue-600">
                  <span className="font-semibold">CMake</span>
                  <span className="ml-1">96.9%</span>
                </span>
              </a>
            </li>
          </ul>
        </div>
      </div>
    </div>
  );
}

export default Sidebar;
