import React from "react";

function RepoMenu() {
  return (
    <ul className="flex flew-row items-center border-b border-gray-300 bg-gray-100 px-8 pt-4">
      <li className="font-semibold ">
        <div className="flex items-center border-b-2 border-red-500 px-1 pb-2">
          <a
            className="flex items-center py-1 px-2 rounded hover:bg-gray-200"
            href="¤"
          >
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-gray-600"
            >
              <path
                fill-rule="evenodd"
                d="M4.72 3.22a.75.75 0 011.06 1.06L2.06 8l3.72 3.72a.75.75 0 11-1.06 1.06L.47 8.53a.75.75 0 010-1.06l4.25-4.25zm6.56 0a.75.75 0 10-1.06 1.06L13.94 8l-3.72 3.72a.75.75 0 101.06 1.06l4.25-4.25a.75.75 0 000-1.06l-4.25-4.25z"
              ></path>
            </svg>
            <span className="ml-2">Code</span>
          </a>
        </div>
      </li>
      <li className="">
        <div className="flex items-center border-b-2 {border-red-500} px-1 pb-2">
          <a
            className="flex items-center py-1 px-2 rounded hover:bg-gray-200"
            href="¤"
          >
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-gray-600"
            >
              <path d="M8 9.5a1.5 1.5 0 100-3 1.5 1.5 0 000 3z"></path>
              <path
                fill-rule="evenodd"
                d="M8 0a8 8 0 100 16A8 8 0 008 0zM1.5 8a6.5 6.5 0 1113 0 6.5 6.5 0 01-13 0z"
              ></path>
            </svg>
            <span className="ml-2">Issues</span>
            <div className="rounded-xl bg-gray-200 px-2 ml-2 font-semibold">
              2.1k
            </div>
          </a>
        </div>
      </li>
      <li className="">
        <div className="flex items-center border-b-2 {border-red-500} px-1 pb-2">
          <a
            className="flex items-center py-1 px-2 rounded hover:bg-gray-200"
            href="¤"
          >
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-gray-600"
            >
              <path
                fill-rule="evenodd"
                d="M7.177 3.073L9.573.677A.25.25 0 0110 .854v4.792a.25.25 0 01-.427.177L7.177 3.427a.25.25 0 010-.354zM3.75 2.5a.75.75 0 100 1.5.75.75 0 000-1.5zm-2.25.75a2.25 2.25 0 113 2.122v5.256a2.251 2.251 0 11-1.5 0V5.372A2.25 2.25 0 011.5 3.25zM11 2.5h-1V4h1a1 1 0 011 1v5.628a2.251 2.251 0 101.5 0V5A2.5 2.5 0 0011 2.5zm1 10.25a.75.75 0 111.5 0 .75.75 0 01-1.5 0zM3.75 12a.75.75 0 100 1.5.75.75 0 000-1.5z"
              ></path>
            </svg>
            <span className="ml-2">Pull requests</span>
            <div className="rounded-xl bg-gray-200 px-2 ml-2 font-semibold">
              75
            </div>
          </a>
        </div>
      </li>
      <li className=" ">
        <div className="flex items-center border-b-2 {border-red-500} px-1 pb-2">
          <a
            className="flex items-center py-1 px-2 rounded hover:bg-gray-200"
            href="¤"
          >
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-gray-600"
            >
              <path
                fill-rule="evenodd"
                d="M0 1.75C0 .784.784 0 1.75 0h12.5C15.216 0 16 .784 16 1.75v3.585a.746.746 0 010 .83v8.085A1.75 1.75 0 0114.25 16H6.309a.748.748 0 01-1.118 0H1.75A1.75 1.75 0 010 14.25V6.165a.746.746 0 010-.83V1.75zM1.5 6.5v7.75c0 .138.112.25.25.25H5v-8H1.5zM5 5H1.5V1.75a.25.25 0 01.25-.25H5V5zm1.5 1.5v8h7.75a.25.25 0 00.25-.25V6.5h-8zm8-1.5h-8V1.5h7.75a.25.25 0 01.25.25V5z"
              ></path>
            </svg>
            <span className="ml-2">Projects</span>
          </a>
        </div>
      </li>
      <li className="">
        <div className="flex items-center border-b-2 {border-red-500} px-1 pb-2">
          <a
            className="flex items-center py-1 px-2 rounded hover:bg-gray-200"
            href="¤"
          >
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-gray-600"
            >
              <path
                fill-rule="evenodd"
                d="M7.467.133a1.75 1.75 0 011.066 0l5.25 1.68A1.75 1.75 0 0115 3.48V7c0 1.566-.32 3.182-1.303 4.682-.983 1.498-2.585 2.813-5.032 3.855a1.7 1.7 0 01-1.33 0c-2.447-1.042-4.049-2.357-5.032-3.855C1.32 10.182 1 8.566 1 7V3.48a1.75 1.75 0 011.217-1.667l5.25-1.68zm.61 1.429a.25.25 0 00-.153 0l-5.25 1.68a.25.25 0 00-.174.238V7c0 1.358.275 2.666 1.057 3.86.784 1.194 2.121 2.34 4.366 3.297a.2.2 0 00.154 0c2.245-.956 3.582-2.104 4.366-3.298C13.225 9.666 13.5 8.36 13.5 7V3.48a.25.25 0 00-.174-.237l-5.25-1.68zM9 10.5a1 1 0 11-2 0 1 1 0 012 0zm-.25-5.75a.75.75 0 10-1.5 0v3a.75.75 0 001.5 0v-3z"
              ></path>
            </svg>
            <span className="ml-2">Security</span>
          </a>
        </div>
      </li>
      <li className="">
        <div className="flex items-center border-b-2 {border-red-500} px-1 pb-2">
          <a
            className="flex items-center py-1 px-2 rounded hover:bg-gray-200"
            href="¤"
          >
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-gray-600"
            >
              <path
                fill-rule="evenodd"
                d="M1.5 1.75a.75.75 0 00-1.5 0v12.5c0 .414.336.75.75.75h14.5a.75.75 0 000-1.5H1.5V1.75zm14.28 2.53a.75.75 0 00-1.06-1.06L10 7.94 7.53 5.47a.75.75 0 00-1.06 0L3.22 8.72a.75.75 0 001.06 1.06L7 7.06l2.47 2.47a.75.75 0 001.06 0l5.25-5.25z"
              ></path>
            </svg>
            <span className="ml-2">Insights</span>
          </a>
        </div>
      </li>
    </ul>
  );
}

export default RepoMenu;
