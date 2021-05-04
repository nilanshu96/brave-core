/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

interface Props {
  white?: boolean
  size: string
}

export function GeminiIcon (props: Props) {
  const fill = props.white ? '#fff' : '#00DCFA'
  const style = { "margin": 3 } as React.CSSProperties;

  return (
    <svg width={props.size} height={props.size} style={style} viewBox="0 0 18 18" fill="none" xmlns="http://www.w3.org/2000/svg">
      <path fill-rule="evenodd" clip-rule="evenodd" d="M5.56944 5.56944C5.90833 2.43556 8.59667 0 11.7633 0C15.2067 0.00245126 17.9975 2.79327 18 6.23667C18 9.40222 15.5644 12.0917 12.4306 12.4306C12.1022 15.5644 9.40333 18 6.23667 18C2.79327 17.9975 0.00245126 15.2067 0 11.7633C0 8.59778 2.43556 5.90833 5.56944 5.56944ZM12.484 11.0325C14.5813 10.7136 16.2279 9.06698 16.5485 6.96797H12.484V11.0325ZM1.45107 11.0325L1.45242 11.0236H5.51559V6.96797C3.41735 7.28771 1.77154 8.93101 1.45242 11.0236H1.45107V11.0325ZM6.2414 16.5485C8.64101 16.5485 10.6788 14.8195 11.0317 12.484H1.45107C1.804 14.8195 3.84178 16.5485 6.2414 16.5485ZM11.0325 6.96797V11.0325H6.96797V6.96797H11.0325ZM6.96797 5.51647H16.5486C16.1957 3.18099 14.1579 1.45195 11.7583 1.45195C9.35868 1.45195 7.32089 3.18099 6.96797 5.51647Z" fill={fill}/>
    </svg>
  )
}
