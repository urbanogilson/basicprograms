import React from "react";
import { readme } from "../constants";
import ReactMarkdown from "react-markdown";
import gfm from "remark-gfm";

function Readme() {
  return (
    <div className="rounded-md border border-[#d0d7de] my-4">
      <div className="readme-container flex items-center bg-[#f6f8fa] ">
        <div className="py-2 flex items-center space-x-2  ml-4">
          <button className="hover:bg-gray-200 p-2">
            <svg
              aria-hidden="true"
              viewBox="0 0 16 16"
              data-view-component="true"
              class="w-4"
            >
              <path
                fill-rule="evenodd"
                d="M2 4a1 1 0 100-2 1 1 0 000 2zm3.75-1.5a.75.75 0 000 1.5h8.5a.75.75 0 000-1.5h-8.5zm0 5a.75.75 0 000 1.5h8.5a.75.75 0 000-1.5h-8.5zm0 5a.75.75 0 000 1.5h8.5a.75.75 0 000-1.5h-8.5zM3 8a1 1 0 11-2 0 1 1 0 012 0zm-1 6a1 1 0 100-2 1 1 0 000 2z"
              ></path>
            </svg>
          </button>
          <a
            href="¤"
            className="font-semibold hover:text-blue-600 hover:underline"
          >
            README.md
          </a>
        </div>
      </div>
      <div className="prose max-w-none px-4">
        <ReactMarkdown remarkPlugins={[gfm]}>{readme}</ReactMarkdown>
      </div>
    </div>
  );
}

export default Readme;
