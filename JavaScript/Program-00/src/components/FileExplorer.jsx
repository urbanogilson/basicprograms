import React from "react";
import { folders } from "../constants";
import Readme from "./Readme";

function FileExplorer() {
  return (
    <div className="file-explorer container mr-8">
      <div className="branch-navigation flex item-center justify-between">
        <div className="flex items-center space-x-3">
          <button className="flex items-center border bg-gray-100 rounded-md border-gray-300 px-4 py-1 hover:bg-gray-200">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="mx-1 w-4 fill-current text-gray-500"
            >
              <path
                fill-rule="evenodd"
                d="M11.75 2.5a.75.75 0 100 1.5.75.75 0 000-1.5zm-2.25.75a2.25 2.25 0 113 2.122V6A2.5 2.5 0 0110 8.5H6a1 1 0 00-1 1v1.128a2.251 2.251 0 11-1.5 0V5.372a2.25 2.25 0 111.5 0v1.836A2.492 2.492 0 016 7h4a1 1 0 001-1v-.628A2.25 2.25 0 019.5 3.25zM4.25 12a.75.75 0 100 1.5.75.75 0 000-1.5zM3.5 3.25a.75.75 0 111.5 0 .75.75 0 01-1.5 0z"
              ></path>
            </svg>
            <span className="font-semibold">main</span>
            <svg
              className="ml-2 w-3"
              fill="none"
              viewBox="0 0 24 24"
              stroke="currentColor"
              strokeWidth={2}
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                d="M19 9l-7 7-7-7"
              />
            </svg>
          </button>
          <a className="flex items-center space-x-1 group" href="¤">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-gray-600 group-hover:text-blue-600"
            >
              <path
                fill-rule="evenodd"
                d="M11.75 2.5a.75.75 0 100 1.5.75.75 0 000-1.5zm-2.25.75a2.25 2.25 0 113 2.122V6A2.5 2.5 0 0110 8.5H6a1 1 0 00-1 1v1.128a2.251 2.251 0 11-1.5 0V5.372a2.25 2.25 0 111.5 0v1.836A2.492 2.492 0 016 7h4a1 1 0 001-1v-.628A2.25 2.25 0 019.5 3.25zM4.25 12a.75.75 0 100 1.5.75.75 0 000-1.5zM3.5 3.25a.75.75 0 111.5 0 .75.75 0 01-1.5 0z"
              ></path>
            </svg>
            <span className="font-semibold group-hover:text-blue-600">128</span>
            <span className="group-hover:text-blue-600">branches</span>
          </a>
          <a className="flex items-center space-x-1 group" href="¤">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-gray-600 group-hover:text-blue-600"
            >
              <path
                fill-rule="evenodd"
                d="M2.5 7.775V2.75a.25.25 0 01.25-.25h5.025a.25.25 0 01.177.073l6.25 6.25a.25.25 0 010 .354l-5.025 5.025a.25.25 0 01-.354 0l-6.25-6.25a.25.25 0 01-.073-.177zm-1.5 0V2.75C1 1.784 1.784 1 2.75 1h5.025c.464 0 .91.184 1.238.513l6.25 6.25a1.75 1.75 0 010 2.474l-5.026 5.026a1.75 1.75 0 01-2.474 0l-6.25-6.25A1.75 1.75 0 011 7.775zM6 5a1 1 0 100 2 1 1 0 000-2z"
              ></path>
            </svg>
            <span className="font-semibold group-hover:text-blue-600">177</span>
            <span className="group-hover:text-blue-600">tags</span>
          </a>
        </div>
        <div className="flex items-center space-x-2">
          <button className="flex items-center border bg-gray-100 rounded-md border-gray-300 px-4 py-1 hover:bg-gray-200">
            <span className="font-semibold">Go to file</span>
          </button>
          <button className="flex items-center border bg-gray-100 rounded-md border-gray-300 px-4 py-1 hover:bg-gray-200">
            <span className="font-semibold">Add file</span>
            <svg
              className="ml-2 w-3"
              fill="none"
              viewBox="0 0 24 24"
              stroke="currentColor"
              strokeWidth={2}
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                d="M19 9l-7 7-7-7"
              />
            </svg>
          </button>
          <button className="flex items-center border bg-green-600 text-white rounded-md border-gray-300 px-4 py-1 hover:bg-green-700">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4 fill-current text-white"
            >
              <path
                fill-rule="evenodd"
                d="M4.72 3.22a.75.75 0 011.06 1.06L2.06 8l3.72 3.72a.75.75 0 11-1.06 1.06L.47 8.53a.75.75 0 010-1.06l4.25-4.25zm6.56 0a.75.75 0 10-1.06 1.06L13.94 8l-3.72 3.72a.75.75 0 101.06 1.06l4.25-4.25a.75.75 0 000-1.06l-4.25-4.25z"
              ></path>
            </svg>
            <span className="ml-2 font-semibold">Code</span>
            <svg
              className="ml-2 w-3"
              fill="none"
              viewBox="0 0 24 24"
              stroke="currentColor"
              strokeWidth={2}
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                d="M19 9l-7 7-7-7"
              />
            </svg>
          </button>
        </div>
      </div>
      <div className="rounded-md border border-[#d0d7de] my-4">
        <div className="commits-container flex items-center justify-between bg-[#f6f8fa] ">
          <div className="py-3 flex items-center space-x-2  ml-4">
            <img
              className="rounded-full w-6 h-6"
              src="https://avatars.githubusercontent.com/u/16805527?s=40&v=4"
              alt="avatar"
            />
            <a className="font-semibold hover:underline" href="¤">
              Gilson Urbano
            </a>
            <a className="hover:text-blue-600 hover:underline" href="¤">
              Update README.md
            </a>
            <span className="bg-gray-200 text-xs hover:bg-blue-300 px-1 w-fit h-fit">
              ...
            </span>
          </div>
          <div className="flex items-center space-x-2 mr-4 text-gray-600">
            <a href="¤">
              <svg
                aria-label="3 / 3 checks OK"
                role="img"
                height="16"
                viewBox="0 0 16 16"
                version="1.1"
                width="16"
                data-view-component="true"
                class="w-4 fill-current text-green-600"
              >
                <path
                  fill-rule="evenodd"
                  d="M13.78 4.22a.75.75 0 010 1.06l-7.25 7.25a.75.75 0 01-1.06 0L2.22 9.28a.75.75 0 011.06-1.06L6 10.94l6.72-6.72a.75.75 0 011.06 0z"
                ></path>
              </svg>
            </a>
            <a className="hover:text-blue-600 hover:underline" href="¤">
              8c37b41
            </a>
            <a className="hover:text-blue-600 hover:underline" href="¤">
              2 hours ago
            </a>
            <a className="flex items-center space-x-1 group" href="¤">
              <svg
                aria-hidden="true"
                viewBox="0 0 16 16"
                data-view-component="true"
                class="w-4 fill-current text-gray-600 group-hover:text-blue-600"
              >
                <path
                  fill-rule="evenodd"
                  d="M1.643 3.143L.427 1.927A.25.25 0 000 2.104V5.75c0 .138.112.25.25.25h3.646a.25.25 0 00.177-.427L2.715 4.215a6.5 6.5 0 11-1.18 4.458.75.75 0 10-1.493.154 8.001 8.001 0 101.6-5.684zM7.75 4a.75.75 0 01.75.75v2.992l2.028.812a.75.75 0 01-.557 1.392l-2.5-1A.75.75 0 017 8.25v-3.5A.75.75 0 017.75 4z"
                ></path>
              </svg>
              <span className="font-semibold group-hover:text-blue-600">
                42,188
              </span>
              <span className="group-hover:text-blue-600">commits</span>
            </a>
          </div>
        </div>
        <div className="files-container border-t border-[#d0d7de] divide-y divide-gray-[#d8dee4]">
          {folders.map((element) => {
            return (
              <div className="flex items-center justify-between px-4 py-2 hover:bg-[#f6f8fa]">
                <div className="w-4/12 flex items-center">
                  {!element.file ? (
                    <svg
                      aria-label="Directory"
                      aria-hidden="true"
                      viewBox="0 0 16 16"
                      data-view-component="true"
                      class="w-4 fill-current text-blue-400 mr-3"
                    >
                      <path d="M1.75 1A1.75 1.75 0 000 2.75v10.5C0 14.216.784 15 1.75 15h12.5A1.75 1.75 0 0016 13.25v-8.5A1.75 1.75 0 0014.25 3H7.5a.25.25 0 01-.2-.1l-.9-1.2C6.07 1.26 5.55 1 5 1H1.75z"></path>
                    </svg>
                  ) : (
                    <svg
                      aria-label="File"
                      aria-hidden="true"
                      viewBox="0 0 16 16"
                      data-view-component="true"
                      class="w-4 mr-3"
                    >
                      <path
                        fill-rule="evenodd"
                        d="M3.75 1.5a.25.25 0 00-.25.25v12.5c0 .138.112.25.25.25h9.5a.25.25 0 00.25-.25V6h-2.75A1.75 1.75 0 019 4.25V1.5H3.75zm6.75.062V4.25c0 .138.112.25.25.25h2.688a.252.252 0 00-.011-.013l-2.914-2.914a.272.272 0 00-.013-.011zM2 1.75C2 .784 2.784 0 3.75 0h6.586c.464 0 .909.184 1.237.513l2.914 2.914c.329.328.513.773.513 1.237v9.586A1.75 1.75 0 0113.25 16h-9.5A1.75 1.75 0 012 14.25V1.75z"
                      ></path>
                    </svg>
                  )}

                  <a className="hover:text-blue-600 hover:underline" href="¤">
                    {element.name}
                  </a>
                </div>
                <div className="w-6/12 truncate">
                  <a className="hover:text-blue-600 hover:underline" href="¤">
                    {element.comment}
                  </a>
                </div>
                <div className="w-2/12  text-right">{element.date}</div>
              </div>
            );
          })}
        </div>
      </div>

      <Readme />
    </div>
  );
}

export default FileExplorer;
